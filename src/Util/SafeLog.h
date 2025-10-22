#pragma once

class SafeLog {
private:
	SafeLog() = default;
	~SafeLog() = default;

public:
	static void Log(const char* log);
	static void LogF(const char* format, ...);
	static void LogF(int size, const char* format, ...);
};