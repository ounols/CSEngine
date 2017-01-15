#pragma once
#ifdef WIN32
//windows include

#include <Windows.h>
#include <gl/glew.h>


#elif __ANDROID__
//android include

#include <jni.h>
#include <android/log.h>

#include <GLES2/gl2.h>


#endif