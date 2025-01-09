#pragma once

struct AppDesc
{
	//shared_ptr<class IExecute> app = nullptr;
	wstring appName = L"GameApplication";
	HINSTANCE hAppInstance = nullptr;
	HWND hMainWnd = nullptr;
	int width = 800;
	int height = 600;
};

class Game
{
public:
	Game(HINSTANCE hInstance);
	virtual ~Game();

public:
	static Game* GetApp();

	WPARAM Run();
	WPARAM Run(AppDesc& appDesc);

	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	bool Initialize();
	bool InitMainWindow();

	void Update();


protected:
	static Game* _gameApp;

	AppDesc _appDesc;

	wstring _appName = L"";
	HINSTANCE _hAppInstance = nullptr;
	HWND _hMainWnd = nullptr;
	int _clientWidth = 800;
	int _clientHeight = 600;
};

