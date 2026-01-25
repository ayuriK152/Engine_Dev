#include "pch.h"
#include "Main.h"
#include "TestScene.h"
#include "DungeonScene.h"
#include "PhysicsTestScene.h"
#include "AnimationTestScene.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int nShowCmd)
{
	AppDesc desc;
	desc.app = make_shared<PhysicsTestScene>();
	desc.mainWndCaption = L"GameApplication";
	desc._4xMsaaState = false;
	desc._4xMsaaQuality = 0;
	desc.clientWidth = 1600;
	desc.clientHeight = 900;

	GAMEAPP->SetAppInst(hInstance, desc);
	GAMEAPP->Run();

	return 0;
}