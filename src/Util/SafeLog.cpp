#include "SafeLog.h"

#ifdef _WIN32

#include <Windows.h>
#include <cstdio>

#elif __ANDROID__
#define  LOG_TAG    "CSEngine"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#include <android/log.h>
#endif
#if defined(__linux__) || defined(__EMSCRIPTEN__) || defined(__APPLE_CC__)
#include <cstdio>
#endif

#ifdef __CSE_EDITOR__

#include "../../Editor/src/Manager/EEngineCore.h"

#endif

void SafeLog::Log(const char* log) {
#ifdef __CSE_EDITOR__
    const auto& editorCore = CSEditor::EEngineCore::getEditorInstance();
    if(editorCore->IsReady()) editorCore->AddLog(log);
    else puts(log);
#elif _WIN32
    OutputDebugStringA(log);
    puts(log);
#elif __ANDROID__
    LOGE(log, 0);
#elif __linux__
    puts(log);
#elif __EMSCRIPTEN__
    puts(log);
#elif __APPLE_CC__
    puts(log);
#endif
}
