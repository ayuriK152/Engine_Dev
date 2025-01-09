#include "pch.h"
#include "Game.h"

WPARAM Game::Run()
{
	MSG msg = { 0 };

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

bool Game::InitMainWindow()
{
	return true;
}

void Game::Update()
{

}
