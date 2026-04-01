#include "pch.h"
#include "Main.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int nShowCmd)
{
	AppDesc desc;
	desc.mainWndCaption = L"Bulb Engine";
	desc._4xMsaaState = false;
	desc._4xMsaaQuality = 0;
	desc.clientWidth = 1600;
	desc.clientHeight = 900;

	SCENE->LoadScene("DungeonScene.xml");

	GAMEAPP->SetAppInst(hInstance, desc);
	GAMEAPP->Run();

	return 0;
}