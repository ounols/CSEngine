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
#include "../../Editor/src/Manager/EditorActionLogger.h"

#endif

void SafeLog::Log(LogLevel level, const char* log) {
    // Prepare level prefix
    const char* levelPrefix = "";
    switch(level) {
        case LogLevel::DEBUG:    levelPrefix = "[DEBUG] "; break;
        case LogLevel::INFO:     levelPrefix = "[INFO] "; break;
        case LogLevel::WARNING:  levelPrefix = "[WARNING] "; break;
        case LogLevel::ERR:      levelPrefix = "[ERROR] "; break;
        case LogLevel::CRITICAL: levelPrefix = "[CRITICAL] "; break;
    }
    
    // Create formatted message with level
    std::string formattedLog = std::string(levelPrefix) + log;
    const char* finalLog = formattedLog.c_str();

#ifdef __CSE_EDITOR__
    const auto& editorCore = CSEditor::EEngineCore::getEditorInstance();
    if(editorCore->IsReady()) {
        editorCore->AddLog(finalLog);
        // Also log to EditorActionLogger for API access
        CSEditor::EditorActionLogger::GetInstance().Log(
            CSEditor::ActionCategory::SYSTEM,
            static_cast<CSEditor::ActionSeverity>(level),
            "SafeLog",
            log // Original log without prefix for structured logging
        );
    } else {
        puts(finalLog);
    }
#elif _WIN32
    OutputDebugStringA(finalLog);
    puts(finalLog);
#elif __ANDROID__
    LOGE(finalLog, 0);
#elif __linux__
    puts(finalLog);
#elif __EMSCRIPTEN__
    puts(finalLog);
#elif __APPLE_CC__
    puts(finalLog);
#endif
}

void SafeLog::vLogF(LogLevel level, const char* format, va_list args) {
    vLogFS(level, 256, format, args);
}

void SafeLog::vLogFS(LogLevel level, int size, const char *format, va_list args) {
    if (size <= 0) size = 256;
    if (size > 8192) size = 8192;

    auto buffer = std::make_unique<char[]>(size);

#ifdef _WIN32
    _vsnprintf_s(buffer.get(), size, _TRUNCATE, format, args);
#else
    vsnprintf(buffer.get(), size, format, args);
#endif

    Log(level, buffer.get());
}

void SafeLog::LogInfo(const char* log) {
    Log(LogLevel::INFO, log);
}

void SafeLog::LogWarn(const char* log) {
    Log(LogLevel::WARNING, log);
}

void SafeLog::LogErr(const char* log) {
    Log(LogLevel::ERR, log);
}

void SafeLog::LogInfof(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vLogF(LogLevel::INFO, format, args);
    va_end(args);
}

void SafeLog::LogWarnf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vLogF(LogLevel::WARNING, format, args);
    va_end(args);
}

void SafeLog::LogErrf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vLogF(LogLevel::ERR, format, args);
    va_end(args);
}

void SafeLog::LogInfof(int size, const char* format, ...) {
    va_list args;
    va_start(args, format);
    vLogFS(LogLevel::INFO, size, format, args);
    va_end(args);
}

void SafeLog::LogWarnf(int size, const char* format, ...) {
    va_list args;
    va_start(args, format);
    vLogFS(LogLevel::WARNING, size, format, args);
    va_end(args);
}

void SafeLog::LogErrf(int size, const char* format, ...) {
    va_list args;
    va_start(args, format);
    vLogFS(LogLevel::ERR, size, format, args);
    va_end(args);
}
