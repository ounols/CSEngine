// GLXBasics.c
// Use GLX to setup OpenGL windows
// Draw eyeballs
// OpenGL SuperBible, 5th Edition
// Nick Haemel

#include <X11/Xlib.h>
#include <OGLDef.h>
#include <GL/glx.h>
#include <Manager/MainProc.h>
#include <MacroDef.h>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/time.h>


// Store all system info in one place
typedef struct RenderContextRec
{
    GLXContext ctx;
    Display *dpy;
    Window win;
    int nWinWidth;
    int nWinHeight;
    int nMousePosX;
    int nMousePosY;

} RenderContext;

MainProc* mainProc = nullptr;

void EarlyInitGLXfnPointers()
{
/*
    glGenVertexArraysAPPLE = (void(*)(GLsizei, const GLuint*))glXGetProcAddressARB((GLubyte*)"glGenVertexArrays");
    glBindVertexArrayAPPLE = (void(*)(const GLuint))glXGetProcAddressARB((GLubyte*)"glBindVertexArray");
    glDeleteVertexArraysAPPLE = (void(*)(GLsizei, const GLuint*))glXGetProcAddressARB((GLubyte*)"glGenVertexArrays");
 glXCreateContextAttribsARB = (GLXContext(*)(Display* dpy, GLXFBConfig config, GLXContext share_context, Bool direct, const int *attrib_list))glXGetProcAddressARB((GLubyte*)"glXCreateContextAttribsARB");
 glXChooseFBConfig = (GLXFBConfig*(*)(Display *dpy, int screen, const int *attrib_list, int *nelements))glXGetProcAddressARB((GLubyte*)"glXChooseFBConfig");
 glXGetVisualFromFBConfig = (XVisualInfo*(*)(Display *dpy, GLXFBConfig config))glXGetProcAddressARB((GLubyte*)"glXGetVisualFromFBConfig");
*/
}
void CreateWindow(RenderContext *rcx)
{
    XSetWindowAttributes winAttribs;
    GLint winmask;
    GLint nMajorVer = 0;
    GLint nMinorVer = 0;
    XVisualInfo *visualInfo;
    GLXFBConfig *fbConfigs;
    int numConfigs = 0;
    static int fbAttribs[] = {
                    GLX_RENDER_TYPE,   GLX_RGBA_BIT,
                    // GLX_X_RENDERABLE,  True,
                    GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
                    GLX_DOUBLEBUFFER,  True,
                    GLX_RED_SIZE,      8,
                    GLX_BLUE_SIZE,     8,
                    GLX_GREEN_SIZE,    8,
		            GLX_DEPTH_SIZE,    24,
                    // GLX_LEVEL,         10,
                    0 };

    EarlyInitGLXfnPointers();

    // Tell X we are going to use the display
    rcx->dpy = XOpenDisplay(NULL);

    // Get Version info
    glXQueryVersion(rcx->dpy, &nMajorVer, &nMinorVer);
    printf("Supported GLX version - %d.%d\n", nMajorVer, nMinorVer);   

    if(nMajorVer == 1 && nMinorVer < 2)
    {
        printf("ERROR: GLX 1.2 or greater is necessary\n");
        XCloseDisplay(rcx->dpy);
        exit(0);
    }
    // Get a new fb config that meets our attrib requirements
    fbConfigs = glXChooseFBConfig(rcx->dpy, DefaultScreen(rcx->dpy), fbAttribs, &numConfigs);
    visualInfo = glXGetVisualFromFBConfig(rcx->dpy, fbConfigs[0]);

    printf("Get a new fb config that meets our attrib requirements\n");

    // Now create an X window
    winAttribs.event_mask = ExposureMask | VisibilityChangeMask | 
                            KeyPressMask | PointerMotionMask    |
                            StructureNotifyMask ;

    winAttribs.border_pixel = 0;
    winAttribs.bit_gravity = StaticGravity;
    winAttribs.colormap = XCreateColormap(rcx->dpy, 
                                          RootWindow(rcx->dpy, visualInfo->screen), 
                                          visualInfo->visual, AllocNone);
    winmask = CWBorderPixel | CWBitGravity | CWEventMask| CWColormap;

    rcx->win = XCreateWindow(rcx->dpy, DefaultRootWindow(rcx->dpy), 20, 20,
                 rcx->nWinWidth, rcx->nWinHeight, 0, 
                             visualInfo->depth, InputOutput,
                 visualInfo->visual, winmask, &winAttribs);

    XMapWindow(rcx->dpy, rcx->win);
    printf("Now create an X window\n");

    // Also create a new GL context for rendering
    GLint attribs[] = {
      GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
      GLX_CONTEXT_MINOR_VERSION_ARB, 0,
      GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
      0 };
    //rcx->ctx = glXCreateContextAttribsARB(rcx->dpy, fbConfigs[0], 0, True, attribs);
    rcx->ctx = glXCreateNewContext(rcx->dpy, fbConfigs[0], GLX_RGBA_TYPE, 0, True);

    int depthBufferSize = 0;

    printf("glXCreateContextAttribsARB()\n");
    glXMakeCurrent(rcx->dpy, rcx->win, rcx->ctx);
    printf("glXMakeCurrent()\n");
#if 0
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }
#endif 

    const GLubyte *s = glGetString(GL_VERSION);
    printf("GL Version = %s\n", s);
    printf("GLSL Version = %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    
}

void SetupGLState(RenderContext *rcx) {
    GLenum err=glewInit();
    if(err!=GLEW_OK) {
        // Problem: glewInit failed, something is seriously wrong.
        std::cout << "glewInit failed: " << glewGetErrorString(err) << std::endl;
        exit(1);
    }
    SAFE_DELETE(mainProc);
    mainProc = new MainProc();

    mainProc->Init(rcx->nWinWidth, rcx->nWinHeight);
}


void Draw(RenderContext *rcx, float elapsedTime) {

    mainProc->Update(elapsedTime);
    mainProc->Render(elapsedTime);
    std::cout << "Rendering\n";
    // Display rendering
    glXSwapBuffers(rcx->dpy, rcx->win); 
}

void Cleanup(RenderContext *rcx)
{
    mainProc->Exterminate();
    SAFE_DELETE(mainProc);

    // Unbind the context before deleting
    glXMakeCurrent(rcx->dpy, None, NULL);

    glXDestroyContext(rcx->dpy, rcx->ctx);
    rcx->ctx = NULL;

    XDestroyWindow(rcx->dpy, rcx->win);
    rcx->win = (Window)NULL;

    XCloseDisplay(rcx->dpy);
    rcx->dpy = 0;
}

float timeGetTime(){
    struct timeval now;
    gettimeofday(&now, nullptr);
    return now.tv_usec/1000.f;
}

int main(int argc, char* argv[])
{
    Bool bWinMapped = False;
    RenderContext rcx;

    float startTime = 0;

    // Set initial window size
    rcx.nWinWidth = 480;
    rcx.nWinHeight = 720;

    // Set initial mouse position
    rcx.nMousePosX = 0;
    rcx.nMousePosY = 0;

    // Setup X window and GLX context
    CreateWindow(&rcx);
    SetupGLState(&rcx);

    startTime = timeGetTime();
    // float elapsedTime = timeGetTime() - startTime;

    // Draw the first frame before checking for messages
    Draw(&rcx, timeGetTime() - startTime);

    // Execute loop the whole time the app runs
    for(;;)
    {
        XEvent newEvent;
        XWindowAttributes winData;

        // Watch for new X events
        XNextEvent(rcx.dpy, &newEvent);

        switch(newEvent.type)
        {
        case UnmapNotify:
        bWinMapped = False;
        break;
        case MapNotify :
        bWinMapped = True;
        case ConfigureNotify:
        XGetWindowAttributes(rcx.dpy, rcx.win, &winData);
        rcx.nWinHeight = winData.height;
        rcx.nWinWidth = winData.width;
        SetupGLState(&rcx);
        break;
        case MotionNotify:
        rcx.nMousePosX = newEvent.xmotion.x;
        rcx.nMousePosY = newEvent.xmotion.y;
        Draw(&rcx, timeGetTime() - startTime);
        break;
        case KeyPress:
        case DestroyNotify:
        Cleanup(&rcx);
        exit(0);
        break;
        }

        if(bWinMapped)
        {
            Draw(&rcx, timeGetTime() - startTime);
        }
    }

    Cleanup(&rcx);
    
    return 0;
}
