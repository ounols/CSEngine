#include "SafeLog.h"

#ifdef WIN32
#include <Windows.h>

#elif __ANDROID__
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,__VA_ARGS__)
#include <android/log.h>
#elif __linux__

#include <iostream>

#endif

void SafeLog::Log(const char* log) {

#ifdef WIN32
	OutputDebugStringA(log);
#elif __ANDROID__
	LOGD(log, 0);
#elif __linux__
	std::cout << log;
#endif
	
}
