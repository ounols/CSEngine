// CSEngine.cpp : ���� ���α׷��� ���� �������� �����մϴ�.
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

// ���� ����:
HINSTANCE hInst; // ���� �ν��Ͻ��Դϴ�.
WCHAR szTitle[MAX_LOADSTRING]; // ���� ǥ���� �ؽ�Ʈ�Դϴ�.
WCHAR szWindowClass[MAX_LOADSTRING]; // �⺻ â Ŭ���� �̸��Դϴ�.

									 // Window dimensions
const GLuint WIDTH = 1920, HEIGHT = 1080;

HDC hdc;
HGLRC hrc;

bool b_isQuit = false;
using namespace CSE;

// �� �ڵ� ��⿡ ��� �ִ� �Լ��� ������ �����Դϴ�.
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
#if defined(MSVC_CMAKE)
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
#endif

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: ���⿡ �ڵ带 �Է��մϴ�.

	// ���� ���ڿ��� �ʱ�ȭ�մϴ�.
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_CSENGINE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// ���� ���α׷� �ʱ�ȭ�� �����մϴ�.
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

	// �⺻ �޽��� �����Դϴ�.
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
//  �Լ�: MyRegisterClass()
//
//  ����: â Ŭ������ ����մϴ�.
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
//   �Լ�: InitInstance(HINSTANCE, int)
//
//   ����: �ν��Ͻ� �ڵ��� �����ϰ� �� â�� ����ϴ�.
//
//   ����:
//
//        �� �Լ��� ���� �ν��Ͻ� �ڵ��� ���� ������ �����ϰ�
//        �� ���α׷� â�� ���� ���� ǥ���մϴ�.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
	hInst = hInstance; // �ν��Ͻ� �ڵ��� ���� ������ �����մϴ�.

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
//  �Լ�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ����:  �� â�� �޽����� ó���մϴ�.
//
//  WM_COMMAND  - ���� ���α׷� �޴��� ó���մϴ�.
//  WM_PAINT    - �� â�� �׸��ϴ�.
//  WM_DESTROY  - ���� �޽����� �Խ��ϰ� ��ȯ�մϴ�.
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
		// �޴� ������ ���� �м��մϴ�.
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
	//	// TODO: ���⿡ hdc�� ����ϴ� �׸��� �ڵ带 �߰��մϴ�.
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


// ���� ��ȭ ������ �޽��� ó�����Դϴ�.
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
