[<img src="https://img.shields.io/badge/-English-green?style=flat"/>](https://github.com/ounols/CSEngine/blob/master/README.md)
[<img src="https://img.shields.io/badge/-한국어-brightgreen?style=flat"/>](https://github.com/ounols/CSEngine/blob/master/README-ko.md)

# CSEngine - Cross Platform C++ Game Engine

<div align=center>
<img src="https://github.com/ounols/CSEngine/raw/master/intro_image2.gif" alt="drawing" width="60%"/>
</div>

[![Android Build](https://github.com/ounols/CSEngine/actions/workflows/build-android.yml/badge.svg?branch=master)](https://github.com/ounols/CSEngine/actions/workflows/build-android.yml)
[![Linux Build](https://github.com/ounols/CSEngine/actions/workflows/build-linux.yml/badge.svg?branch=master)](https://github.com/ounols/CSEngine/actions/workflows/build-linux.yml)
[![MacOS Build](https://github.com/ounols/CSEngine/actions/workflows/build-macos.yml/badge.svg?branch=master)](https://github.com/ounols/CSEngine/actions/workflows/build-macos.yml)
[![Web Build](https://github.com/ounols/CSEngine/actions/workflows/build-web.yml/badge.svg?branch=master)](https://github.com/ounols/CSEngine/actions/workflows/build-web.yml)
[![Windows Build](https://github.com/ounols/CSEngine/actions/workflows/build-windows.yml/badge.svg?branch=master)](https://github.com/ounols/CSEngine/actions/workflows/build-windows.yml)
![last-commit](https://img.shields.io/github/last-commit/ounols/CSEngine?style=flat-square) ![GitHub](https://img.shields.io/github/license/ounols/CSEngine?style=flat-square) [![CodeFactor](https://www.codefactor.io/repository/github/ounols/csengine/badge/master)](https://www.codefactor.io/repository/github/ounols/csengine/overview/master) </br>
CSEngine is a cross-platform 3D game engine.

`💡 As it is under development, it is not yet suitable for practical use.`

## Requirements

![intro-image](https://github.com/ounols/CSEngine/raw/master/intro_image.png)

### In all environments

* CMake 3.15 or higher
* C++14 compatible compiler
* GLEW 2.1.0 or higher
* GLFW 3.3 or higher
* OpenGL 4.3 or higher

### Android

* Gradle 4.1.3 or higher
* NDK 21.2.6472646 or higher

### Web

* emsdk 3.0.0 or higher

## Building

CSEngine is built using CMake. In the case of Windows, it is possible to build with Visual Studio because it supports MSVC build through CMake.

### Windows

Put the GLEW library folder in the `External/glew` folder within the project.
<br><br>

#### Visual Studio IDE

Open the project via `platforms/Windows/CSEngine.sln`
<br><br>

#### CMake (MSVC)

Set the compiler settings to Visual Studio. (For setting method, refer to the help of each IDE such as VS, Clion, etc.)
</p>

    cmake -DCMAKE_BUILD_TYPE=Debug [Project Path]/CSEngine/platforms/Windows
Create make files.
</p>

	cmake --build [Directory containing make files]
Build through the created make files.
<br><br>

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
<br><br>

### macOS

    brew install cmake ninja glfw
Install external libraries required at build.

    cd [Project Path]/CSEngine/platforms/MacOS
	mkdir build
	cd build
	cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Debug ../
Create make files.

    cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Debug ../
Build through the created make files.


## Stability

Various stabilization tests such as build tests for various platforms and several unit tests will be added.
|Branch|Linux|Windows|MacOS|Android|Web|Architecture|
|--|--|--|--|--|--|--|
|master|[![Linux Build](https://github.com/ounols/CSEngine/actions/workflows/build-linux.yml/badge.svg?branch=master)](https://github.com/ounols/CSEngine/actions/workflows/build-linux.yml)|[![Windows Build](https://github.com/ounols/CSEngine/actions/workflows/build-windows.yml/badge.svg?branch=master)](https://github.com/ounols/CSEngine/actions/workflows/build-windows.yml)|[![MacOS Build](https://github.com/ounols/CSEngine/actions/workflows/build-macos.yml/badge.svg?branch=master)](https://github.com/ounols/CSEngine/actions/workflows/build-macos.yml)|[![Android Build](https://github.com/ounols/CSEngine/actions/workflows/build-android.yml/badge.svg?branch=master)](https://github.com/ounols/CSEngine/actions/workflows/build-android.yml)|[![Web Build](https://github.com/ounols/CSEngine/actions/workflows/build-web.yml/badge.svg?branch=master)](https://github.com/ounols/CSEngine/actions/workflows/build-web.yml)|[<img src='https://img.shields.io/travis/com/ounols/CSEngine?style=flat-square'/>](https://app.travis-ci.com/github/ounols/CSEngine/branches) |
|build_testing||||||[<img src='https://img.shields.io/travis/com/ounols/CSEngine/build_testing?style=flat-square'/>](https://app.travis-ci.com/github/ounols/CSEngine/branches) |

## See more

Review the following documentation:
* [`web demo`](https://ounols.github.io/CSEngine-Demo/): This site is a simple web demo of the engine ported to WebGL 2.0.
* [`documentation`](https://doc-engine.ounols.kr): Documentation that details of the structure and function of CSEngine.
* [`to-do list`](https://www.notion.so/CSEngine-Todo-List-7ee24caed138466e83d81d2867b1f109): Documentation that contains the to-do list of the project and notes during development.


## 3rd Party Copyright & Licence

- GLEW : Modified BSD License, the Mesa 3-D License (MIT), Khronos License (MIT)
- GLAD : The MIT License (MIT)
- GLFW : zlib License
- IMGUI : The MIT License (MIT)
- stb : The MIT License (MIT)
- Squirrel Script : The MIT License (MIT)
- kuba--/zip : The Unlicense
