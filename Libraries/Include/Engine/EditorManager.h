#pragma once

class BULB_API EditorManager
{
	DECLARE_SINGLE(EditorManager);
public:
	~EditorManager();

	void Init();

public:
	void Play();
	void Stop();

	bool IsOnPlay() { return _isOnPlay; }

	void SetEditorWindowText(string text);

private:
	void RestoreObjectComponents(shared_ptr<GameObject> go, GameObjectSnapshot objectSnapshot);

private:
	bool _isOnPlay = false;
	string _currentWindowText;

	vector<GameObjectSnapshot> _objectSnapshots;
	vector<ComponentSnapshot> _compSnapshots;
};

