//#define GLFW_INCLUDE_GLCOREARB

#include "../../src/Manager/MainProc.h"
#include "../../src/MacroDef.h"
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sys/time.h>
#include <cmath>

using namespace CSE;

long long timeGetTime() {
    using namespace std::chrono;
    milliseconds ms = duration_cast< milliseconds >(
            system_clock::now().time_since_epoch()
    );
    return ms.count();
}

int main(void) {
    GLFWwindow *window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    // glfwWindowHint(GLFW_CLIENT_API,GLFW_OPENGL_ES_API);
    // glfwWindowHint(GLFW_CONTEXT_CREATION_API,GLFW_EGL_CONTEXT_API);
    // glfwDefaultWindowHints();

    //OpenGL ES
//    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
//    glfwWindowHint(GLFW_SAMPLES, 4);


    //OpenGL core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int width = 1280;
    int height = 720;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "CSEngine", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwGetFramebufferSize(window, &width, &height);
    glfwMakeContextCurrent(window);
    if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize OpenGL context\n";
        return -1;
    }

    printf("GL_VERSION  : %s\n", glGetString(GL_VERSION));
    printf("GL_RENDERER : %s\n", glGetString(GL_RENDERER));

    MainProc *mainProc = new MainProc();

    mainProc->Init(width, height);

    auto elapsedTime = timeGetTime();

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
    SAFE_DELETE(mainProc);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}