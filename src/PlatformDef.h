#pragma once

#if defined(__ANDROID__) || defined(__EMSCRIPTEN__)
#define __CSE_ES__
#elif defined(_WIN32) || defined(__linux__)
#define __CSE_DESKTOP__
#endif