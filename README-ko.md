
[<img src="https://img.shields.io/badge/-English-green?style=flat"/>](https://github.com/ounols/CSEngine/blob/master/README.md)
[<img src="https://img.shields.io/badge/-한국어-brightgreen?style=flat"/>](https://github.com/ounols/CSEngine/blob/master/README-ko.md)

# CSEngine - Cross Platform C++ Game Engine

![intro-image2](https://github.com/ounols/CSEngine/raw/master/intro_image2.gif)</br>
[![Android Build](https://github.com/ounols/CSEngine/actions/workflows/build-android.yml/badge.svg?branch=master)](https://github.com/ounols/CSEngine/actions/workflows/build-android.yml)
[![Linux Build](https://github.com/ounols/CSEngine/actions/workflows/build-linux.yml/badge.svg?branch=master)](https://github.com/ounols/CSEngine/actions/workflows/build-linux.yml)
[![MacOS Build](https://github.com/ounols/CSEngine/actions/workflows/build-macos.yml/badge.svg?branch=master)](https://github.com/ounols/CSEngine/actions/workflows/build-macos.yml)
[![Web Build](https://github.com/ounols/CSEngine/actions/workflows/build-web.yml/badge.svg?branch=master)](https://github.com/ounols/CSEngine/actions/workflows/build-web.yml)
[![Windows Build](https://github.com/ounols/CSEngine/actions/workflows/build-windows.yml/badge.svg?branch=master)](https://github.com/ounols/CSEngine/actions/workflows/build-windows.yml)
![last-commit](https://img.shields.io/github/last-commit/ounols/CSEngine?style=flat-square) ![GitHub](https://img.shields.io/github/license/ounols/CSEngine?style=flat-square) [![CodeFactor](https://www.codefactor.io/repository/github/ounols/csengine/badge/master)](https://www.codefactor.io/repository/github/ounols/csengine/overview/master) </br>
CSEngine은 크로스 플랫폼 기반 3D 게임 엔진입니다.

`💡 아직 개발 중이기 때문에 실사용으로 적합하지 않은 단계입니다.`

## 요구 사항

![intro-image](https://github.com/ounols/CSEngine/raw/master/intro_image.png)

### 공통 사항

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

## 빌드

CSEngine은 CMake를 이용해 빌드합니다. 윈도우는 CMake를 통해 MSVC로 빌드하도록 설계되었기 때문에 Visual Studio에서도 사용이 가능합니다.

### Windows

GLEW 라이브러리 폴더를 프로젝트의 `External/glew` 폴더에 넣습니다.
<br><br>

#### Visual Studio IDE

`platforms/Windows/CSEngine.sln` 를 통해 프로젝트를 엽니다.
<br><br>

#### CMake (MSVC)

컴파일러 설정을 Visual Studio로 설정합니다. (설정 방법은 VS, Clion 등 각 IDE의 도움말을 참고해주세요.)
</p>

    cmake -DCMAKE_BUILD_TYPE=Debug [Project Path]/CSEngine/platforms/Windows
make 파일들을 생성합니다.
</p>

	cmake --build [Directory containing make files]
생성된 make 파일들을 통해 cmake 빌드를 진행합니다.
<br><br>

### Linux

    apt install build-essential cmake
    apt install mesa-common-dev libglfw3 libglfw3-dev libglew-dev
빌드에 필요한 외부 라이브러리를 설치합니다.
</p>

	cd [Project Path]/CSEngine/platforms/Linux
	mkdir build
	cd build
	cmake -DCMAKE_BUILD_TYPE=Debug ../
make 파일들을 생성합니다.
</p>


	cmake --build [Directory containing make files]
생성된 make 파일들을 통해 cmake 빌드를 진행합니다.
<br><br>

### macOS

    brew install cmake ninja glfw
빌드에 필요한 외부 라이브러리를 설치합니다.

    cd [Project Path]/CSEngine/platforms/MacOS
	mkdir build
	cd build
	cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Debug ../
make 파일들을 생성합니다.

    cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Debug ../
생성된 make 파일들을 통해 cmake 빌드를 진행합니다.

## 테스트

다양한 플랫폼에 대한 빌드 및 단위 테스트와 같은 다양한 안정화 테스트가 추가 될 예정입니다.
|Branch|Linux|Windows|MacOS|Android|Web|Architecture|
|--|--|--|--|--|--|--|
|master|[![Linux Build](https://github.com/ounols/CSEngine/actions/workflows/build-linux.yml/badge.svg?branch=master)](https://github.com/ounols/CSEngine/actions/workflows/build-linux.yml)|[![Windows Build](https://github.com/ounols/CSEngine/actions/workflows/build-windows.yml/badge.svg?branch=master)](https://github.com/ounols/CSEngine/actions/workflows/build-windows.yml)|[![MacOS Build](https://github.com/ounols/CSEngine/actions/workflows/build-macos.yml/badge.svg?branch=master)](https://github.com/ounols/CSEngine/actions/workflows/build-macos.yml)|[![Android Build](https://github.com/ounols/CSEngine/actions/workflows/build-android.yml/badge.svg?branch=master)](https://github.com/ounols/CSEngine/actions/workflows/build-android.yml)|[![Web Build](https://github.com/ounols/CSEngine/actions/workflows/build-web.yml/badge.svg?branch=master)](https://github.com/ounols/CSEngine/actions/workflows/build-web.yml)|[<img src='https://img.shields.io/travis/com/ounols/CSEngine?style=flat-square'/>](https://app.travis-ci.com/github/ounols/CSEngine/branches) |
|build_testing||||||[<img src='https://img.shields.io/travis/com/ounols/CSEngine/build_testing?style=flat-square'/>](https://app.travis-ci.com/github/ounols/CSEngine/branches) |

## 더보기

아래의 문서들을 통해 자세한 내용을 확인하실 수 있습니다.
* [`web demo`](https://ounols.github.io/CSEngine-Demo/): WebGL 2.0으로 포팅된 간단한 웹 데모 입니다.
* [`documentation`](https://doc-engine.ounols.kr): CSEngine의 구조와 기능을 자세히 설명하는 위키입니다.
* [`to-do list`](https://www.notion.so/CSEngine-Todo-List-7ee24caed138466e83d81d2867b1f109): CSEngine의 할일 리스트이자 개발 진행하며 메모한 내용을 정리하였습니다.

## 3rd Party Copyright & Licence

- GLEW : Modified BSD License, the Mesa 3-D License (MIT), Khronos License (MIT)
- GLAD : The MIT License (MIT)
- GLFW : zlib License
- IMGUI : The MIT License (MIT)
- stb : The MIT License (MIT)
- Squirrel Script : The MIT License (MIT)
- kuba--/zip : The Unlicense
