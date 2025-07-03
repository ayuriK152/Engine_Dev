#include "pch.h"
#include "Main.h"
#include "Engine/GameApplication.h"
#include "TestScene.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int nShowCmd)
{
	AppDesc desc;
	desc.app = make_shared<TestScene>();
	desc.mainWndCaption = L"GameApplication";
	desc._4xMsaaState = false;
	desc._4xMsaaQuality = 0;
	desc.clientWidth = 1600;
	desc.clientHeight = 900;

	GAMEAPP->SetAppInst(hInstance, desc);
	GAMEAPP->Run();

	return 0;
}