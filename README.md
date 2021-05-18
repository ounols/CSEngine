
# CSEngine - Cross Platform C++ Game Engine

![intro-image](https://s3.us-west-2.amazonaws.com/secure.notion-static.com/a90dcf3b-a8e6-4412-a31c-291bbaec8a39/Untitled.png?X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Credential=AKIAT73L2G45O3KS52Y5/20210518/us-west-2/s3/aws4_request&X-Amz-Date=20210518T094537Z&X-Amz-Expires=86400&X-Amz-Signature=78641c0a61fa63e40a0c80dc3de451613ed84a9d6a638f7dbdcc3d9f292a3835&X-Amz-SignedHeaders=host&response-content-disposition=filename%20=%22Untitled.png%22)</br>
![platform](https://img.shields.io/badge/platform-windows_%7C_linux_%7C_android-00B274?style=flat-square) ![Travis (.com)](https://img.shields.io/travis/com/ounols/CSEngine?style=flat-square) ![last-commit](https://img.shields.io/github/last-commit/ounols/CSEngine?style=flat-square) ![GitHub](https://img.shields.io/github/license/ounols/CSEngine?style=flat-square) </br>
CSEngine is a cross-platform 3D game engine.

`💡 As it is under development, it is not yet suitable for practical use.`

## Requirements

### In all environments

* CMake 3.15 or higher
* C++14 compatible compiler
* GLEW 2.1.0 or higher
* GLFW 3.3 or higher
* OpenGL 

### Android

* Gradle 4.1.3 or higher
* NDK 21.2.6472646 or higher

## Building

CSEngine is built using CMake. In the case of Windows, it is possible to build with Visual Studio because it supports MSVC build through CMake.

### Windows

Put the GLEW library folder in the `External/glew` folder within the project.

#### Visual Studio IDE

Open the project via `platforms/Windows/CSEngine.sln`

#### CMake (MSVC)

Set the compiler settings to Visual Studio. (For setting method, refer to the help of each IDE such as VS, Clion, etc.)
</p>

    cmake -DCMAKE_BUILD_TYPE=Debug [Project Path]/CSEngine/platforms/Windows
Create make files.
</p>

	cmake --build [Directory containing make files]
Build through the created make files.

### Linux

    apt install build-essential cmake
    apt install mesa-common-dev libglfw3 libglfw3-dev libglew-dev
Install external libraries required at build. 
</p>

	cd [Project Path]/CSEngine/platforms/Linux
	mkdir build
	cd build
	cmake -DCMAKE_BUILD_TYPE=Debug ../
Create make files.
</p>


	cmake --build [Directory containing make files]
Build through the created make files.

## Stability

Currently, only Linux build tests are available. In the future, various stabilization tests such as build tests for various platforms and several unit tests will be added.
|Branch|Build status|
|--|--|
|master|![Travis (.com)](https://img.shields.io/travis/com/ounols/CSEngine?style=flat-square) |

## See more

Review the following documentation:
* [`wiki`](https://github.com/ounols/CSEngine/wiki): Documentation that details of the structure and function of CSEngine.

## 3rd Party Copyright & Licence 

- GLEW : Modified BSD License, the Mesa 3-D License (MIT), Khronos License (MIT)
- GLAD : The MIT License (MIT)
- GLFW : zlib License
- IMGUI : The MIT License (MIT)
- lodepng : zlib License
- Squirrel Script : The MIT License (MIT)
- kuba--/zip : The Unlicense
