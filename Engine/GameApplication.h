#pragma once

class GameApplication
{
	DECLARE_SINGLE(GameApplication);
public:
	HINSTANCE	GetAppInst()const;
	void		SetAppInst(HINSTANCE hInstance, AppDesc appDesc);

	AppStatus	GetAppStatus()const;
	void		SetAppStatus(AppStatus appStatus);

public:
	int Run();
	bool Initialize();
	void PreUpdate();
	void Update();

	void ExitApplication();

private:
	HINSTANCE _hAppInst = nullptr; // application instance handle

	AppStatus _appStatus;
};

