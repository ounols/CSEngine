// GLXBasics.c
// Use GLX to setup OpenGL windows
// Draw eyeballs
// OpenGL SuperBible, 5th Edition
// Nick Haemel

#include "../../src/Manager/MainProc.h"
#include "../../src/MacroDef.h"
#define GLFW_INCLUDE_ES3

#include <GLFW/glfw3.h>
#include <emscripten.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/time.h>
#include <math.h>

using namespace CSE;

float timeGetTime() {
    long ms; // Milliseconds
    int s;  // Seconds
    struct timespec spec;

    clock_gettime(CLOCK_REALTIME, &spec);

    s = spec.tv_sec * 1000;
    ms = round(spec.tv_nsec / 1.0e6); // Convert nanoseconds to milliseconds

    return ms + s;
}
float elapsedTime = 0.f;
GLFWwindow* window = nullptr;
MainProc* mainProc = nullptr;

void mainLoop() {
    elapsedTime += 1;
    float deltaTime = elapsedTime;
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

int main(void) {

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    // glfwWindowHint(GLFW_CLIENT_API,GLFW_OPENGL_ES_API);
    // glfwWindowHint(GLFW_CONTEXT_CREATION_API,GLFW_EGL_CONTEXT_API);
    // glfwDefaultWindowHints();

    //OpenGL ES
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
//    glfwWindowHint(GLFW_SAMPLES, 4);


    //OpenGL core
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//    glfwWindowHint(GLFW_SAMPLES, 4);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int width = 500;
    int height = 400;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "CSEngine", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

//     glewExperimental = GL_TRUE;
//     GLenum err=glewInit();
//     if(err!=GLEW_OK) {
//         // Problem: glewInit failed, something is seriously wrong.
//         std::cout << "glewInit failed: " << glewGetErrorString(err) << std::endl;
//         return -1;
//     }

    printf("GL_VERSION  : %s\n", glGetString(GL_VERSION));
    printf("GL_RENDERER : %s\n", glGetString(GL_RENDERER));

    mainProc = new MainProc();
    mainProc->Init(width, height);
    elapsedTime = 0;
    /* Loop until the user closes the window */
    emscripten_set_main_loop(&mainLoop, 0, 1);
    SAFE_DELETE(mainProc);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}