
cmake_minimum_required(VERSION 3.4.1)
project(CSEngine.editor)

SET(CMAKE_VERBOSE_MAKEFILE true)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Debug)

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

file(GLOB SQUIRREL_SRC
        "../../../External/Squirrel/lib/sqstdlib/*.*"
        "../../../External/Squirrel/lib/squirrel/*.*")

file(GLOB ZIP_SRC
        "../../../External/zip/*.*")

file(GLOB IMGUI_SRC
        "../../../External/imgui/*.*")


add_library( SquirrelLib
        SHARED
        ${SQUIRREL_SRC})
add_library( ZipLib
        SHARED
        ${ZIP_SRC})
add_library( CSEngineCore
        SHARED
        ${CSENGINE_SRC})
#add_library( ImguiLib
#        SHARED
#        ${IMGUI_SRC})


add_executable(CSEngine.editor main.cpp ${CSENGINE_EDITOR_SRC} ${IMGUI_SRC})

#헤더파일 위치
include_directories(/usr/include)
include_directories(/usr/local/include)
#include_directories(../../../src)
include_directories(../../../External/Squirrel/include)
include_directories(../../../External/zip)
include_directories(../../../External/imgui)

#target_link_libraries(CSEngine.editor X11)
target_link_libraries(CSEngine.editor glfw)
#target_link_libraries(CSEngine.editor GLESv2)
target_link_libraries(CSEngine.editor GL)
#target_link_libraries(CSEngine.editor m)
target_link_libraries(CSEngine.editor CSEngineCore)
target_link_libraries(CSEngine.editor SquirrelLib)
target_link_libraries(CSEngine.editor ZipLib)
#target_link_libraries(CSEngine.editor ImguiLib)