#pragma once
#include <cstdarg>

class SafeLog {
private:
	SafeLog() = default;
	~SafeLog() = default;

	enum class LogLevel {
		DEBUG = 0,      // Detailed debug information
		INFO = 1,       // Normal information
		WARNING = 2,    // Warning messages
		ERR = 3,        // Error messages (renamed from ERROR to avoid Windows macro conflict)
		CRITICAL = 4    // Critical errors / crashes
	};

private:
	static void Log(LogLevel level, const char* log);

	// va_list versions (internal use)
	static void vLogF(LogLevel level, const char* format, va_list args);
	static void vLogFS(LogLevel level, int size, const char* format, va_list args);

public:
	static void LogInfo(const char* log);
	static void LogWarn(const char* log);
	static void LogErr(const char* log);

	static void LogInfof(const char* format, ...);
	static void LogWarnf(const char* format, ...);
	static void LogErrf(const char* format, ...);

	static void LogInfof(int size, const char* format, ...);
	static void LogWarnf(int size, const char* format, ...);
	static void LogErrf(int size, const char* format, ...);
};
