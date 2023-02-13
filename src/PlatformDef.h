#pragma once

#if defined(__ANDROID__) || defined(__EMSCRIPTEN__) || defined(IOS)
#define __CSE_ES__
#elif defined(_WIN32) || defined(__linux__) || defined(__APPLE_CC__)
#define __CSE_DESKTOP__
#endif