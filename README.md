[<img src="https://img.shields.io/badge/-English-green?style=flat"/>](https://github.com/ounols/CSEngine/blob/master/README.md)
[<img src="https://img.shields.io/badge/-한국어-brightgreen?style=flat"/>](https://github.com/ounols/CSEngine/blob/master/README-ko.md)

# CSEngine - Cross Platform C++ Game Engine

![intro-image](https://github.com/ounols/CSEngine/raw/master/intro_image.png)</br>
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
* [`to-do list`](https://www.notion.so/CSEngine-Todo-List-7ee24caed138466e83d81d2867b1f109): Documentation that contains the to-do list of the project and notes during development.


## 3rd Party Copyright & Licence

- GLEW : Modified BSD License, the Mesa 3-D License (MIT), Khronos License (MIT)
- GLAD : The MIT License (MIT)
- GLFW : zlib License
- IMGUI : The MIT License (MIT)
- lodepng : zlib License
- Squirrel Script : The MIT License (MIT)
- kuba--/zip : The Unlicense
