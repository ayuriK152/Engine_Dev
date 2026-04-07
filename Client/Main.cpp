#include "pch.h"
#include "Main.h"
#include "EnemyScript.h"
#include "PlayerScript.h"
#include "TPVCamera.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int nShowCmd)
{
	AppDesc desc;
	desc.mainWndCaption = L"Bulb Engine";
	desc._4xMsaaState = false;
	desc._4xMsaaQuality = 0;
	desc.clientWidth = 1600;
	desc.clientHeight = 900;

	SCENE->LoadScene("MainScene.xml");

	APP->SetAppInst(hInstance, desc);

	//REGISTER_COMPONENT(EnemyScript);
	//REGISTER_COMPONENT(PlayerScript);
	//REGISTER_COMPONENT(TPVCamera);
	APP->Run();

	return 0;
}