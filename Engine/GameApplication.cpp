#include "pch.h"
#include "GameApplication.h"

HINSTANCE GameApplication::GetAppInst() const
{
	return _hAppInst;
}

void GameApplication::SetAppInst(HINSTANCE hInstance, AppDesc appDesc)
{
	_hAppInst = hInstance;
	GRAPHIC->SetAppDesc(appDesc);
}

AppStatus GameApplication::GetAppStatus() const
{
	return _appStatus;
}

void GameApplication::SetAppStatus(AppStatus appStatus)
{
	_appStatus = appStatus;
}

int GameApplication::Run()
{
	if (!Initialize())
		return 0;

	MSG msg = { 0 };

	TIME->Reset();

	while (msg.message != WM_QUIT)
	{
		// If there are Window messages then process them.
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// Otherwise, do animation/game stuff.
		else
		{
			Update();
		}
	}

	return (int)msg.wParam;
}

// �� �ھ�ý��ۺ� �ʱ�ȭ ����
bool GameApplication::Initialize()
{
	if (!TIME->Initialize())
		return false;
	if (!GRAPHIC->Initialize())
		return false;

	return true;
}

void GameApplication::Update()
{
	TIME->Tick();

	if (!_appStatus.appPaused)
	{
		GRAPHIC->CalculateFrameStats();
		GRAPHIC->Update();
		GRAPHIC->Render();
	}
	else
	{
		Sleep(100);
	}
}