#include "pch.h"
#include "BulbApplication.h"

BulbApplication* BulbApplication::s_instance = nullptr;

BulbApplication::~BulbApplication()
{
#ifdef PRINT_DEBUG_CONSOLE_LOG
	cout << "Released - ResourceManager\n";
#endif
}

BulbApplication* BulbApplication::GetInstance()
{
	if (s_instance == nullptr)
		s_instance = new BulbApplication();
	return s_instance;
}

Bulb::ProcessResult BulbApplication::Delete()
{
	if (s_instance != nullptr) {
		delete s_instance;
		s_instance = nullptr;
		return Bulb::ProcessResult::SUCCESS;
	}
	return Bulb::ProcessResult::FAILED_INSTANCE_NOT_FOUND;
}

HINSTANCE BulbApplication::GetAppInst() const
{
	return _hAppInst;
}

void BulbApplication::SetAppInst(HINSTANCE hInstance, AppDesc appDesc)
{
	_hAppInst = hInstance;
	GRAPHIC->SetAppDesc(appDesc);
}

AppStatus BulbApplication::GetAppStatus() const
{
	return _appStatus;
}

void BulbApplication::SetAppStatus(AppStatus appStatus)
{
	_appStatus = appStatus;
}

int BulbApplication::Run()
{
	if (!Init())
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
			TIME->Tick();

			PreUpdate();
			Update();
		}
	}

	ExitApplication();

	return (int)msg.wParam;
}

// 각 코어시스템별 초기화 진행
bool BulbApplication::Init()
{
	if (!TIME->Init())
		return false;
	if (!GRAPHIC->Init())
		return false;

	ComponentFactory::Init();
	THREAD->Init();
	RENDER->Init();
	FILEIO->Init();
	UI->Init();
	PHYSICS->Init();
	SCENE->Init();
	INPUTM->Init();
	Utils::Init();

	DEBUG->Init();
	EDITOR->Init();

	return true;
}

void BulbApplication::PreUpdate()
{
	if (!_appStatus.appPaused) {
		DEBUG->PreUpdate();
		PHYSICS->PreUpdate();
		RENDER->PreUpdate();
	}
}

void BulbApplication::Update()
{
	if (!_appStatus.appPaused) {
		INPUTM->Update();
		GRAPHIC->Update();
		PHYSICS->Update();
		RENDER->Update();
		ANIMATION->Update();
		ENGINESTAT->Update();
		UI->Update();
		ENGINEGUI->Update();
		DEBUG->Update();

		GRAPHIC->LateUpdate();
		PHYSICS->LateUpdate();

		RENDER->Render();
	}
	else
	{
		Sleep(100);
	}
}

void BulbApplication::ExitApplication()
{
	INPUTM->Delete();
	EDITOR->Delete();
	DEBUG->Delete();
	SCENE->Delete();
	UI->Delete();
	FILEIO->Delete();
	RESOURCE->Delete();
	ENGINEGUI->Delete();
	RENDER->Delete();
	PHYSICS->Delete();
	THREAD->Delete();

	GRAPHIC->Delete();
	TIME->Delete();

	PostQuitMessage(0);
}
