#pragma once

struct AppDesc
{
	//shared_ptr<class IExecute> app = nullptr;
	wstring _mainWndCaption = L"GameApplication";
	bool msaaState = false;
	int msaaQuality = 1;
	int clientWidth = 800;
	int clientHeight = 600;
};

struct AppStatus
{
	bool      appPaused = false;  // is the application paused?
	bool      minimized = false;  // is the application minimized?
	bool      maximized = false;  // is the application maximized?
	bool      resizing = false;   // are the resize bars being dragged?
	bool      fullscreenState = false;// fullscreen enabled
};