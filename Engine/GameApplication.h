#pragma once

class GameApplication
{
	DECLARE_SINGLE(GameApplication);
public:
	HINSTANCE	GetAppInst()const;
	void		SetAppInst(HINSTANCE hInstance);

	AppStatus	GetAppStatus()const;
	void		SetAppStatus(AppStatus appStatus);

public:
	int Run();
	bool Initialize();
	void Update();

private:
	HINSTANCE _hAppInst = nullptr; // application instance handle

	AppStatus _appStatus;
};

