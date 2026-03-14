#pragma once

class SceneManager
{
	DECLARE_SINGLE(SceneManager);
public:
	~SceneManager();

	void Init();

public:
	void LoadScene(string sceneName);

private:
	bool _isInitialized = false;

	string _queuedSceneName = "";
};

