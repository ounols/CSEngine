# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

PROJECT(CSEditor.app)
SET(CMAKE_VERBOSE_MAKEFILE true)
SET(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Debug)

link_directories(/usr/lib)
link_directories(/usr/lib32)
link_directories(/usr/local/lib)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

add_compile_definitions(__CSE_EDITOR__)

include_directories(${OPENGL_INCLUDE_DIR})

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


# file(GLOB GL3W_SRC "gl3w/*.c")


# add_library( CSEngineLib
#               SHARED
#               ${CSENGINE_SRC} )


add_library( SquirrelLib
        SHARED
        ${SQUIRREL_SRC})
#add_library( CSEngineCore
#        SHARED
#        ${CSENGINE_SRC})


add_executable(CSEditor.app glad/src/glad.c ${CSENGINE_SRC} ${CSENGINE_EDITOR_SRC} ${IMGUI_SRC})

#헤더파일 위치
include_directories(/usr/include)
include_directories(/usr/local/include)
include_directories(../../../External/Squirrel/include)
include_directories(../../../External/zip)
include_directories(../../../External/imgui)
include_directories(glad/include)
include_directories(${GLEW_INCLUDE_DIRS})

target_link_libraries(CSEditor.app glfw)
target_link_libraries(CSEditor.app SquirrelLib)
target_link_libraries(CSEditor.app OpenGL::GL)
#target_link_libraries(CSEditor.app CSEngineCore)
