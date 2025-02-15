#pragma once

struct AppDesc
{
	shared_ptr<class IExecute> app = nullptr;
	wstring mainWndCaption = L"GameApplication";
	bool _4xMsaaState = false;
	int _4xMsaaQuality = 0;
	int clientWidth = 800;
	int clientHeight = 600;
};

struct AppStatus
{
	bool		appPaused = false;
	bool		minimized = false;
	bool		maximized = false;
	bool		resizing = false;
	bool		fullscreenState = false;
};