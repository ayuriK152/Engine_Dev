#include "pch.h"
#include "Main.h"
#include "Engine/GameApplication.h"
#include "TestScript.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int nShowCmd)
{
	AppDesc desc;
	desc.app = make_shared<TestScript>();
	desc.mainWndCaption = L"GameApplication";
	desc._4xMsaaState = false;
	desc._4xMsaaQuality = 0;
	desc.clientWidth = 800;
	desc.clientHeight = 600;

	GAMEAPP->SetAppInst(hInstance, desc);
	GAMEAPP->Run();

	return 0;
}