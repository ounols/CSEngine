// GLXBasics.c
// Use GLX to setup OpenGL windows
// Draw eyeballs
// OpenGL SuperBible, 5th Edition
// Nick Haemel

#include "../../src/Manager/MainProc.h"
#include "../../src/MacroDef.h"
// #include <GL/glew.h>
#define GLFW_INCLUDE_ES3

#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/time.h>
#include <math.h>


float timeGetTime() {
    long ms; // Milliseconds
    int s;  // Seconds
    struct timespec spec;

    clock_gettime(CLOCK_REALTIME, &spec);

    s = spec.tv_sec * 1000;
    ms = round(spec.tv_nsec / 1.0e6); // Convert nanoseconds to milliseconds

    return ms + s;
}

int main(void) {
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    // glfwWindowHint(GLFW_CLIENT_API,GLFW_OPENGL_ES_API);
    // glfwWindowHint(GLFW_CONTEXT_CREATION_API,GLFW_EGL_CONTEXT_API);
    // glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    int width = 1280;
    int height = 720;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "Hello World", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // glewExperimental = true;
    // GLenum err=glewInit();
    // if(err!=GLEW_OK) {
    //     // Problem: glewInit failed, something is seriously wrong.
    //     std::cout << "glewInit failed: " << glewGetErrorString(err) << std::endl;
    //     return -1;
    // }

    printf("GL_VERSION  : %s\n", glGetString(GL_VERSION));
    printf("GL_RENDERER : %s\n", glGetString(GL_RENDERER));

    MainProc* mainProc = new MainProc();

    mainProc->Init(width, height);
    mainProc->ResizeWindow(width, height);

    float elapsedTime = timeGetTime();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        float deltaTime = timeGetTime() - elapsedTime;
        // std::cout << deltaTime <<'\n';
        /* Render here */
        mainProc->Update(deltaTime);
        mainProc->Render(deltaTime);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        // glfwSwapInterval(0);

        /* Poll for and process events */
        glfwPollEvents();
    }
    mainProc->Exterminate();
    SAFE_DELETE(mainProc);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}