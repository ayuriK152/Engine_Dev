#pragma once

struct AppDesc
{
	//shared_ptr<class IExecute> app = nullptr;
	wstring _mainWndCaption = L"GameApplication";
	HINSTANCE hAppInstance = nullptr;
	HWND hMainWnd = nullptr;
	bool msaaState = false;
	int msaaQuality = 1;
	int width = 800;
	int height = 600;
};