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

private:
	void InitializeScene();

	void ReadGameObjectData(XMLElement* objsElem, shared_ptr<GameObject> parent);
	void WriteGameObjectData(XMLElement* objsElem, shared_ptr<GameObject> go);

private:
	bool _isInitialized = false;

	string _currentSceneName = "";

	string _currentScenePath = "";
	string _queuedScenePath = "";
};

