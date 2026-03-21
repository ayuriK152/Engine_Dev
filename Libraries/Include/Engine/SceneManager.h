#pragma once

class SceneManager
{
	DECLARE_SINGLE(SceneManager);
public:
	~SceneManager();

	void Init();

public:
	void LoadScene();
	void LoadScene(string sceneName, bool isFullPath = false);

	void SaveScene(bool saveAs = false);
	void SaveScene(string scenePath, bool isFullPath = false);

	void WriteGameObjectData(XMLElement* objsElem, shared_ptr<GameObject> go);

private:
	void InitializeScene();

private:
	bool _isInitialized = false;

	string _currentSceneName = "";

	string _currentScenePath = "";
	string _queuedScenePath = "";
};

