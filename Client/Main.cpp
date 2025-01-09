#include "pch.h"
#include "Main.h"
#include "Engine/Game.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int nShowCmd)
{
	Game game(hInstance);
	AppDesc desc;
	desc.appName = L"GameApplication";
	desc.hAppInstance = hInstance;
	desc.hMainWnd = NULL;
	desc.width = 800;
	desc.height = 600;

	Game::GetApp()->Run(desc);

	return 0;
}