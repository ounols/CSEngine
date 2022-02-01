# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

PROJECT(CSEngineWeb)
SET_PROPERTY(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS TRUE)
SET(CMAKE_VERBOSE_MAKEFILE true)
#set(CMAKE_CXX_FLAGS "-o index.js -s DISABLE_EXCEPTION_CATCHING=0 -s DEMANGLE_SUPPORT=1 -s SAFE_HEAP=1 --bind -s WASM=1 -O3 -s LEGACY_GL_EMULATION=0  -s GL_UNSAFE_OPTS=0 -s ASSERTIONS=1 -s GL_ASSERTIONS=1 -s INVOKE_RUN=0  -std=c++14 -s USE_WEBGL2=1 -s FULL_ES3=1 -s USE_GLFW=3 -s OFFSCREENCANVAS_SUPPORT=1 --preload-file textures --preload-file shaders --preload-file fonts")
#SET(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE Debug)

#link_directories(/usr/lib)
#link_directories(/usr/lib32)
#link_directories(/usr/local/lib)

#find_package(glfw3 REQUIRED)
#find_package(OpenGL REQUIRED)



find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

#find_package(PkgConfig REQUIRED)
#pkg_search_module(GLFW REQUIRED glfw3)
#include_directories(${GLFW_INCLUDE_DIRS})
#target_link_libraries(CSEngine ${GLFW_LIDRARIES})

file(GLOB CSENGINE_SRC
                        "../../src/*.*"
                        "../../src/*/*.*"
                        "../../src/*/*/*.*"
                        "../../src/*/*/*/*.*"
                        "../../src/*/*/*/*/*.*"
                        "../../src/*/*/*/*/*/*.*"
#                        "../../src/*/*/*/*/*/*/*.*"
        )

file(GLOB SQUIRREL_SRC
                        "../../External/Squirrel/lib/sqstdlib/*.*"
                        "../../External/Squirrel/lib/squirrel/*.*")


# file(GLOB GL3W_SRC "gl3w/*.c")



# add_library( CSEngineLib
#               SHARED
#               ${CSENGINE_SRC} )


add_library( SquirrelLib
            SHARED
            ${SQUIRREL_SRC})


add_executable(CSEngineWeb CSEngine_Web_glfw.cpp ${CSENGINE_SRC})

#헤더파일 위치
#include_directories(/usr/include)
#include_directories(/usr/local/include)
#include_directories(../../src)
include_directories(../../External/Squirrel/include)
# include_directories(/gl3w)


target_link_libraries(CSEngineWeb glfw)
target_link_libraries(CSEngineWeb SquirrelLib)

set_target_properties(CSEngineWeb
        PROPERTIES SUFFIX ".html"
        LINK_FLAGS " --bind -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -O3 -std=c++14 -s ALLOW_MEMORY_GROWTH=1 -s EXCEPTION_CATCHING_ALLOWED=[..] -s USE_WEBGL2=1 -g4 --preload-file Assets.zip")
