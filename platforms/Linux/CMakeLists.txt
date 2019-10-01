# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

PROJECT(CSEngine.app)
SET(CMAKE_VERBOSE_MAKEFILE true)
#SET(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE Debug)

link_directories(/usr/lib)
link_directories(/usr/lib32)
link_directories(/usr/local/lib)

#find_package(glfw3 REQUIRED)
#find_package(OpenGL REQUIRED)



#find_package(OpenGL REQUIRED)
#include_directories(${OPENGL_INCLUDE_DIR})

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

file(GLOB ZIP_SRC
        "../../External/zip/*.*")

# file(GLOB GL3W_SRC "gl3w/*.c")



# add_library( CSEngineLib
#               SHARED
#               ${CSENGINE_SRC} )


add_library( SquirrelLib
            SHARED
            ${SQUIRREL_SRC})
add_library( ZipLib
        SHARED
        ${ZIP_SRC})


add_executable(CSEngine.app CSEngine_Linux_glfw.cpp ${CSENGINE_SRC})

#헤더파일 위치
include_directories(/usr/include)
include_directories(/usr/local/include)
#include_directories(../../src)
include_directories(../../External/Squirrel/include)
include_directories(../../External/zip)
# include_directories(/gl3w)


target_link_libraries(CSEngine.app X11)
# target_link_libraries(CSEngine.app GLEW)
target_link_libraries(CSEngine.app glfw)
target_link_libraries(CSEngine.app GLESv2)
# target_link_libraries(CSEngine.app GL)
target_link_libraries(CSEngine.app m)
# target_link_libraries(CSEngine.app CSEngineLib)
target_link_libraries(CSEngine.app SquirrelLib)
target_link_libraries(CSEngine.app ZipLib)