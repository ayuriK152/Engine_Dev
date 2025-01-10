#include "pch.h"
#include "Game.h"

Game* Game::_gameApp = nullptr;

Game::~Game()
{

}

WPARAM Game::Run(AppDesc& appDesc)
{
	_appDesc = appDesc;
	return Run();
}

WPARAM Game::Run()
{
	MSG msg = { 0 };

	Initialize();

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{

		}
	}

	return msg.wParam;
}

// 윈도우 생성 인자로 넘겨줄 콜백 함수
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return GAME->MsgProc(hwnd, msg, wParam, lParam);
}

// 실질적 윈도우 메세지 처리부
LRESULT Game::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_MENUCHAR:
		return MAKELRESULT(0, MNC_CLOSE);

	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;
	case WM_KEYUP:
		if (wParam == VK_ESCAPE)
		{
			PostQuitMessage(0);
		}

		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

// 윈도우, DirectX 초기 설정부
bool Game::Initialize()
{
	if (!InitMainWindow())
		return false;

	return true;
}

// 윈도우 설정 함수
bool Game::InitMainWindow()
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = _appDesc.hAppInstance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"MainWnd";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	RECT R = { 0, 0, _appDesc.width, _appDesc.height };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	_appDesc.hMainWnd = CreateWindow(L"MainWnd", _appDesc.appName.c_str(),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, _appDesc.hAppInstance, 0);
	if (!_appDesc.hMainWnd)
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	ShowWindow(_appDesc.hMainWnd, SW_SHOW);
	UpdateWindow(_appDesc.hMainWnd);

	return true;
}

void Game::Update()
{

}
