
cmake_minimum_required(VERSION 3.4.1)
project(CSEngine.editor)

SET(CMAKE_VERBOSE_MAKEFILE true)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Debug)
add_compile_definitions(__CSE_EDITOR__)

find_package(glfw3 REQUIRED)

link_directories(/usr/lib)
link_directories(/usr/lib32)
link_directories(/usr/local/lib)

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

add_library(SquirrelLib
        SHARED
        ${SQUIRREL_SRC})

add_executable(CSEngine.editor ${CSENGINE_SRC} ${CSENGINE_EDITOR_SRC} ${IMGUI_SRC})

#헤더파일 위치
include_directories(/usr/include)
include_directories(/usr/local/include)
include_directories(../../../External/Squirrel/include)
include_directories(../../../External/zip)
include_directories(../../../External/imgui)

target_link_libraries(CSEngine.editor X11)
target_link_libraries(CSEngine.editor GLEW)
target_link_libraries(CSEngine.editor glfw)
target_link_libraries(CSEngine.editor GL)
target_link_libraries(CSEngine.editor m)
target_link_libraries(CSEngine.editor SquirrelLib)
target_link_libraries(CSEngine.editor ${CMAKE_DL_LIBS})