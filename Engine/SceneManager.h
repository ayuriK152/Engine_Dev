#pragma once

class BULB_API SceneManager
{
	friend class BulbApplication;

private:
	SceneManager() = default;
	~SceneManager();

	void Init();
	void PreUpdate();

	void InitializeScene();

	void ReadGameObjectData(XMLElement* objsElem, shared_ptr<GameObject> parent);
	void WriteGameObjectData(XMLElement* objsElem, shared_ptr<GameObject> go);

	void ReadUIData(XMLElement* uisElem, shared_ptr<UIElement> parent);

public:
	SceneManager(const SceneManager& rhs) = delete;
	SceneManager& operator=(const SceneManager& rhs) = delete;

	static SceneManager* GetInstance();
	static Bulb::ProcessResult Delete();

	void LoadScene();
	void LoadScene(string sceneName, bool isFullPath = false);
	void LoadSceneOnRender(string scenePath);

	void SaveScene(bool saveAs = false);
	void SaveScene(string scenePath, bool isFullPath = false);

private:
	static SceneManager* s_instance;

	bool _isInitialized = false;

	string _currentSceneName = "";

	string _currentScenePath = "";
	string _queuedScenePath = "";
};

