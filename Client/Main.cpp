#include "pch.h"
#include "Main.h"
#include "Engine/GameApplication.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int nShowCmd)
{
	//AppDesc desc;
	//desc._mainWndCaption = L"GameApplication";
	//desc.hAppInstance = hInstance;
	//desc.hMainWnd = NULL;
	//desc.width = 800;
	//desc.height = 600;

	GRAPHIC->SetAppInst(hInstance);
	GRAPHIC->Run();

	return 0;
}