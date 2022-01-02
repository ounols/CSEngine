#include "SafeLog.h"

#ifdef WIN32
#include <Windows.h>
#include <cstdio>

#elif __ANDROID__
#define  LOG_TAG    "CSEngine"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#include <android/log.h>
#elif __linux__

#include <cstdio>

#endif

void SafeLog::Log(const char* log) {

#ifdef WIN32
	OutputDebugStringA(log);
    puts(log);
#elif __ANDROID__
	LOGE(log, 0);
#elif __linux__
	puts(log);
#endif
	
}
