// Dear ImGui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

// For Emscripten
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <functional>
#define GLFW_INCLUDE_ES3
static std::function<void()>            MainLoopForEmscriptenP;
static void MainLoopForEmscripten()     { MainLoopForEmscriptenP(); }
#define EMSCRIPTEN_MAINLOOP_BEGIN       MainLoopForEmscriptenP = [&]()
#define EMSCRIPTEN_MAINLOOP_END         ; emscripten_set_main_loop(MainLoopForEmscripten, 0, true)
#else
#define EMSCRIPTEN_MAINLOOP_BEGIN
#define EMSCRIPTEN_MAINLOOP_END
#endif
#if defined(__APPLE_CC__)
#include <glad/glad.h>
#include <iostream>
#elif defined(_WIN64)
#include <gl/glew.h>
#include <crtdbg.h>
#elif defined(__linux__)
#include <GL/glew.h>
#endif

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <cstdio>

#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES3/gl3.h>
#endif

#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#include "Objects/MainDocker.h"
#include "Manager/EEngineCore.h"

#define __CSE_REFLECTION_ENABLE__

#include "../../src/Manager/ReflectionMgr.h"

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

#pragma comment(lib, "opengl32.lib")
#if !defined(_WIN64) && !defined(MSVC_CMAKE)
#pragma comment(lib, "../../../External/glew/lib/Win32/glew32.lib")
#pragma comment(lib, "../../../../External/glfw/lib/Win32/lib-vc2022/glfw3_mt.lib")
#elif defined(_WIN64) && !defined(MSVC_CMAKE)
#pragma comment(lib, "../../../../External/glew/lib/Win64/glew32.lib")
#pragma comment(lib, "../../../../External/glfw/lib/Win64/lib-vc2022/glfw3_mt.lib")
#endif
//#pragma comment(lib, "glut32.lib")
//#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "winmm.lib")

#if defined(_DEBUG) && !defined(MSVC_CMAKE) && !defined(_WIN64)
#pragma comment(lib, "../../../../External/Squirrel/lib/Windows/Win32/squirrelD.lib")
#pragma comment(lib, "../../../../External/Squirrel/lib/Windows/Win32/sqstdlibD.lib")
#elif defined(_DEBUG) && !defined(MSVC_CMAKE) && defined(_WIN64)
#pragma comment(lib, "../../../../External/Squirrel/lib/Windows/Win64/squirrelD.lib")
#pragma comment(lib, "../../../../External/Squirrel/lib/Windows/Win64/sqstdlibD.lib")
#elif !defined(_DEBUG) && !defined(MSVC_CMAKE) // Release
#pragma comment(lib, "../../../../External/Squirrel/lib/Windows/squirrel.lib")
#pragma comment(lib, "../../../../External/Squirrel/lib/Windows/sqstdlib.lib")
#endif

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

const char* setupOpenGLVersion() {
#if defined(__EMSCRIPTEN__)
    const char* glsl_version = "#version 300 es";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(IMGUI_IMPL_OPENGL_ES2)
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif
    return glsl_version;
}

GLFWwindow* createWindow(unsigned int width, unsigned int height) {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return nullptr;

    GLFWwindow* window = glfwCreateWindow(width, height, "CSEditor", NULL, NULL);
    if (window == nullptr)
        return nullptr;
        
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    return window;
}

bool initializeOpenGL() {
#if defined(__APPLE_CC__)
    if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize OpenGL context\n";
        return false;
    }
#elif defined(_WIN64) || defined(__linux__)
    glewInit();
#endif
    return true;
}

void setupImGui(const char* glsl_version) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
#ifndef __EMSCRIPTEN__
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
#endif

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    ImGui_ImplGlfw_InitForOpenGL(glfwGetCurrentContext(), true);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

void renderFrame(CSEditor::MainDocker* mainDocker, bool& show_demo_window, bool& show_another_window, 
                const ImVec4& clear_color) {
    glfwPollEvents();

    ImGuiIO& io = ImGui::GetIO();
    if (io.DeltaTime <= 0.0f) io.DeltaTime = 0.00001f;
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    mainDocker->SetUI();

    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    if (show_another_window) {
        ImGui::Begin("Another Window", &show_another_window);
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            show_another_window = false;
        ImGui::End();
    }

    ImGui::Render();
    GLFWwindow* window = glfwGetCurrentContext();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, 
                 clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

void updateEngineCore() {
    const auto& core = CSEditor::EEngineCore::getEditorInstance();
    const auto& state = core->CheckInvokeState();
    core->BindPreviewFramebuffer();
    
    switch (state) {
        case CSEditor::EEngineCore::START:
            core->StartPreviewCore();
            break;
        case CSEditor::EEngineCore::RESIZE:
            core->ResizePreviewCore();
            break;
        case CSEditor::EEngineCore::STOP:
            core->StopPreviewCore();
            core->ResizePreviewCore();
            break;
    }

    if (core->IsPreview()) {
        core->UpdatePreviewCore();
        core->RenderPreviewCore();
    }
    else if (core->IsRender()) {
        core->Render();
        core->UpdateTransforms();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void cleanup(CSEditor::MainDocker* mainDocker, GLFWwindow* window) {
    SAFE_DELETE(mainDocker);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}

int main(int, char**) {
#if defined(_WIN64)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#if defined(MSVC_CMAKE)
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
#endif
#endif

    const char* glsl_version = setupOpenGLVersion();
    
    unsigned int width = 1280;
    unsigned int height = 720;
    GLFWwindow* window = createWindow(width, height);
    if (window == nullptr)
        return 1;

    if (!initializeOpenGL())
        return -1;

    setupImGui(glsl_version);

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    auto* mainDocker = new CSEditor::MainDocker();
    CSEditor::EEngineCore::getEditorInstance()->Init(width, height);
    CSEditor::EEngineCore::getEditorInstance()->InitPreviewFramebuffer();
    CSEditor::EEngineCore::getEditorInstance()->Update(0);

#ifdef __EMSCRIPTEN__
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    EMSCRIPTEN_MAINLOOP_BEGIN {
#else
    while (!glfwWindowShouldClose(window)) {
#endif
        renderFrame(mainDocker, show_demo_window, show_another_window, clear_color);
        updateEngineCore();
        glfwSwapBuffers(window);
    }
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif

    cleanup(mainDocker, window);
    return 0;
}
