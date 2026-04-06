#pragma once

class BULB_API BulbApplication
{
private:
	BulbApplication() = default;
	~BulbApplication();

	bool Init();
	void PreUpdate();
	void Update();

public:
	BulbApplication(const BulbApplication& rhs) = delete;
	BulbApplication& operator=(const BulbApplication& rhs) = delete;

	static BulbApplication* GetInstance();
	static Bulb::ProcessResult Delete();

	// Run Bulb Application
	int Run();

	// Exit Bulb Application
	void ExitApplication();

	HINSTANCE	GetAppInst()const;
	void		SetAppInst(HINSTANCE hInstance, AppDesc appDesc);

	AppStatus	GetAppStatus()const;
	void		SetAppStatus(AppStatus appStatus);

private:
	static BulbApplication* s_instance;

	HINSTANCE _hAppInst = nullptr; // application instance handle

	AppStatus _appStatus;
};

