#pragma once

struct BULB_API AppDesc
{
	wstring mainWndCaption = L"GameApplication";
	bool _4xMsaaState = false;
	int _4xMsaaQuality = 0;
	int clientWidth = 1280;
	int clientHeight = 720;
};

struct BULB_API AppStatus
{
	bool		appPaused = false;
	bool		minimized = false;
	bool		maximized = false;
	bool		resizing = false;
	bool		fullscreenState = false;
};