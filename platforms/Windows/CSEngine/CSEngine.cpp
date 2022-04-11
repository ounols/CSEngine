// CSEngine.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"
#include "CSEngine.h"

#include <timeapi.h>
#include <thread>

//#include <gl/GL.h>
//#include <gl/GLU.h>
//#include <gl/glut.h>
#include <GL/glew.h>
#include <crtdbg.h>
#include "../../../src/Manager/MainProc.h"
#include "../../../src/MacroDef.h"

#if !defined(_WIN64) && !defined(MSVC_CMAKE)
#pragma comment(lib, "../../../External/glew/lib/Win32/glew32.lib")
#elif defined(_WIN64) && !defined(MSVC_CMAKE)
#pragma comment(lib, "../../../External/glew/lib/Win64/glew32.lib")
#endif
#pragma comment(lib, "opengl32.lib")
//#pragma comment(lib, "glut32.lib")
//#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "winmm.lib")

#if defined(_DEBUG) && !defined(MSVC_CMAKE) && !defined(_WIN64)
#pragma comment(lib, "../../../External/Squirrel/lib/Windows/Win32/squirrelD.lib")
#pragma comment(lib, "../../../External/Squirrel/lib/Windows/Win32/sqstdlibD.lib")
#elif defined(_DEBUG) && !defined(MSVC_CMAKE) && defined(_WIN64)
#pragma comment(lib, "../../../External/Squirrel/lib/Windows/Win64/squirrelD.lib")
#pragma comment(lib, "../../../External/Squirrel/lib/Windows/Win64/sqstdlibD.lib")
#elif !defined(_DEBUG) && !defined(MSVC_CMAKE) // Release
#pragma comment(lib, "../../../External/Squirrel/lib/Windows/squirrel.lib")
#pragma comment(lib, "../../../External/Squirrel/lib/Windows/sqstdlib.lib")
#endif


#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst; // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING]; // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING]; // 기본 창 클래스 이름입니다.

									 // Window dimensions
const GLuint WIDTH = 1920, HEIGHT = 1080;

HDC hdc;
HGLRC hrc;

bool b_isQuit = false;
using namespace CSE;

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

void Update(MainProc* mainProc, DWORD dwStartTime);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow) {

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 여기에 코드를 입력합니다.

	// 전역 문자열을 초기화합니다.
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_CSENGINE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 응용 프로그램 초기화를 수행합니다.
	if (!InitInstance(hInstance, nCmdShow)) {
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CSENGINE));

	MainProc* mainProc = new MainProc();

	//init GLEW
	glewInit();

	char* GL_version = (char*)glGetString(GL_VERSION);
	char* GL_vendor = (char*)glGetString(GL_VENDOR);
	char* GL_renderer = (char*)glGetString(GL_RENDERER);

	std::string buf;

	buf += std::string("Version : ") + (char*)glGetString(GL_VERSION) + '\n';
	buf += std::string("Vendor : ") + (char*)glGetString(GL_VENDOR) + '\n';
	buf += std::string("Renderer : ") + (char*)glGetString(GL_RENDERER) + '\n';

	OutputDebugStringA(buf.c_str());

	//GL20Lib::setupGLGraphics(WIDTH, HEIGHT);
	mainProc->Init(WIDTH, HEIGHT);
		
	MSG msg;
	DWORD dwStartTime = timeGetTime();

//	std::thread tUpdate(Update, mainProc, dwStartTime);

	// 기본 메시지 루프입니다.
	while (!b_isQuit) {

		if (GetAsyncKeyState(VK_ESCAPE)) {
			b_isQuit = true;
		}

		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else {
			//GL20Lib::renderFrame(static_cast<float>(timeGetTime() - dwStartTime));
			auto deltaTime = static_cast<float>(timeGetTime() - dwStartTime);
			mainProc->Update(deltaTime);
			mainProc->Render(0);
            glFinish();

			Sleep(1);
		}

	}
//	tUpdate.join();
	SAFE_DELETE(mainProc);

	return static_cast<int>(msg.wParam);
}


//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance) {
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CSENGINE));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_CSENGINE);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}


//
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
	hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

	HWND hWnd = CreateWindowW(szWindowClass, L"CSEngine - WIN32", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, WIDTH, HEIGHT, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd) {
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}


//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적:  주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_CREATE:
		PIXELFORMATDESCRIPTOR pfd;

		int nPixelFormat;

		hdc = GetDC(hWnd);

		memset(&pfd, 0, sizeof(pfd));

		pfd.nSize = sizeof(pfd);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 32;

		nPixelFormat = ChoosePixelFormat(hdc, &pfd);
		SetPixelFormat(hdc, nPixelFormat, &pfd);

		hrc = wglCreateContext(hdc);
		wglMakeCurrent(hdc, hrc);

		break;

	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		// 메뉴 선택을 구문 분석합니다.
		switch (wmId) {
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
		break;
	//case WM_PAINT: {
	//	PAINTSTRUCT ps;
	//	HDC hdc = BeginPaint(hWnd, &ps);
	//	// TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다.
	//	EndPaint(hWnd, &ps);
	//}
	//	break;
	case WM_DESTROY:
		b_isQuit = true;
		wglMakeCurrent(hdc, nullptr);
		wglDeleteContext(hrc);
		ReleaseDC(hWnd, hdc);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);
	switch (message) {
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void Update(MainProc* mainProc, DWORD dwStartTime) {

	while (!b_isQuit) {
		float deltaTime = static_cast<float>(timeGetTime() - dwStartTime);
		mainProc->Update(deltaTime);

		Sleep(1);

	}

	SafeLog::Log("The rendering thread exterminates!\n");
	
}
