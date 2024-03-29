# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
             native-lib

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             src/main/cpp/native-lib.cpp )

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

#if (${ANDROID_PLATFORM_LEVEL} LESS 12)
#    message(FATAL_ERROR "OpenGL 2 is not supported before API level 11 \
#                      (currently using ${ANDROID_PLATFORM_LEVEL}).")
#    return()
#elseif (${ANDROID_PLATFORM_LEVEL} LESS 18)
#    add_definitions("-DDYNAMIC_ES3")
#    set(GL3STUB_SRC src/main/cpp/gl3stub.c)
#    message(WARNING  "OpenGL 2")
#    set(OPENGL_LIB GLESv2)
#else ()
#    message(WARNING  "OpenGL 3")
##    set(OPENGL_LIB GLESv3)
#endif (${ANDROID_PLATFORM_LEVEL} LESS 12)

add_library( CSEngineLib
              SHARED
              ${GL3STUB_SRC} ${CSENGINE_SRC} src/main/cpp/CSEngineAndroidBinder.cpp )

add_library( SquirrelLib
            SHARED
            ${SQUIRREL_SRC})

#헤더파일 위치
include_directories(../../../src)
include_directories(src/main/cpp)
#Squirrel 헤더파일 위치
include_directories(../../../External/Squirrel/include)

#add_library( CSEngineBinderLib
#              SHARED
#              src/main/cpp/CSEngineAndroidBinder.cpp )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       native-lib

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib} )

target_link_libraries(CSEngineLib
                     ${OPENGL_LIB}
                     android
                     log
                     EGL
                     SquirrelLib
                     GLESv3
                     ${log-lib})

set(CMAKE_C_FLAGS "-Wno-error=format-security")