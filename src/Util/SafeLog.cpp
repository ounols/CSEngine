#include "SafeLog.h"
#include <memory>
#include <cstdarg>

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

void SafeLog::LogF(const char* format, ...) {
    char buffer[256];
    va_list args;
    va_start(args, format);
#ifdef _WIN32
    _vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, format, args);
#else
    vsnprintf(buffer, sizeof(buffer), format, args);
#endif
    va_end(args);

    Log(buffer);
}

void SafeLog::LogF(int size, const char *format, ...) {
    if (size <= 0) size = 256;
    if (size > 8192) size = 8192;

    auto buffer = std::make_unique<char[]>(size);

    va_list args;
    va_start(args, format);
#ifdef _WIN32
    _vsnprintf_s(buffer.get(), size, _TRUNCATE, format, args);
#else
    vsnprintf(buffer.get(), size, format, args);
#endif
    va_end(args);

    Log(buffer.get());
}
