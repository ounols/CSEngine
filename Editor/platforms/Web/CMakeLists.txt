# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

PROJECT(CSEditor.web)
SET_PROPERTY(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS TRUE)
SET(CMAKE_VERBOSE_MAKEFILE true)
SET(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Debug)

add_compile_definitions(__CSE_EDITOR__)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

#find_package(PkgConfig REQUIRED)
#pkg_search_module(GLFW REQUIRED glfw3)
#include_directories(${GLFW_INCLUDE_DIRS})
#target_link_libraries(CSEngine ${GLFW_LIDRARIES})

file(GLOB CSENGINE_EDITOR_SRC
        "../../src/*.*"
        "../../src/*/*.*"
        "../../src/*/*/*.*"
        "../../src/*/*/*/*.*"
        "../../src/*/*/*/*/*.*"
        "../../src/*/*/*/*/*/*.*"
)

file(GLOB CSENGINE_SRC
        "../../../src/*.*"
        "../../../src/*/*.*"
        "../../../src/*/*/*.*"
        "../../../src/*/*/*/*.*"
        "../../../src/*/*/*/*/*.*"
        "../../../src/*/*/*/*/*/*.*"
        "../../../src/*/*/*/*/*/*/*.*"
)
# 엔진 기본값으로 지정된 소스파일 제거
list(FILTER CSENGINE_SRC EXCLUDE REGEX ".*CSEngine/src/Manager/EngineCore.")

file(GLOB SQUIRREL_SRC
        "../../../External/Squirrel/lib/sqstdlib/*.*"
        "../../../External/Squirrel/lib/squirrel/*.*")

file(GLOB IMGUI_SRC
        "../../../External/imgui/*.*")

add_library( SquirrelLib
        SHARED
        ${SQUIRREL_SRC})


add_executable(CSEditor.web ${CSENGINE_SRC} ${CSENGINE_EDITOR_SRC} ${IMGUI_SRC})

#헤더파일 위치
#include_directories(/usr/include)
#include_directories(/usr/local/include)
#include_directories(../../src)
include_directories(../../../External/Squirrel/include)
include_directories(../../../External/imgui)
# include_directories(/gl3w)


target_link_libraries(CSEditor.web glfw)
target_link_libraries(CSEditor.web SquirrelLib)

set_target_properties(CSEditor.web
        PROPERTIES SUFFIX ".html"
        LINK_FLAGS " --bind -s USE_GLFW=3 -s MAXIMUM_MEMORY=4GB -s FULL_ES3=1 -s SAFE_HEAP=0 -s ASSERTIONS=1 -s WASM=1 -O3 -std=c++14 -s ALLOW_MEMORY_GROWTH=1 -s DISABLE_EXCEPTION_CATCHING=0 -s USE_WEBGL2=1 -g4 --profiling-funcs --preload-file Assets.zip")
