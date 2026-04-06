#pragma once

class BULB_API EditorManager
{
	friend class BulbApplication;

public:
	EditorManager() = default;
	~EditorManager();

	void Init();

public:
	EditorManager(const EditorManager& rhs) = delete;
	EditorManager& operator=(const EditorManager& rhs) = delete;

	static EditorManager* GetInstance();
	static Bulb::ProcessResult Delete();

	void Play();
	void Stop();

	bool IsOnPlay() { return _isOnPlay; }

	void SetEditorWindowText(string text);

private:
	void RestoreObjectComponents(shared_ptr<GameObject> go, GameObjectSnapshot objectSnapshot);

private:
	static EditorManager* s_instance;

	bool _isOnPlay = false;
	string _currentWindowText;

	vector<GameObjectSnapshot> _objectSnapshots;
	vector<ComponentSnapshot> _compSnapshots;
};

