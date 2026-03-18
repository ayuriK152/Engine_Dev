#pragma once

class SceneManager
{
	DECLARE_SINGLE(SceneManager);
public:
	~SceneManager();

	void Init();

public:
	void LoadScene();
	void LoadScene(string sceneName);
	void SaveScene();

private:
	bool _isInitialized = false;

	string _currentSceneName = "";
	string _queuedSceneName = "";
};

