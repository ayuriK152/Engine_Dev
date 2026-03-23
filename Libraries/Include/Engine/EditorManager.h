#pragma once

class EditorManager
{
	DECLARE_SINGLE(EditorManager);
public:
	~EditorManager();

public:
	void Play();
	void Stop();

	bool IsOnPlay() { return _isOnPlay; }

private:
	void StoreState();
	void RestoreState();

	void RestoreObjectComponents(shared_ptr<GameObject> go, GameObjectSnapshot objectSnapshot);

private:
	bool _isOnPlay = false;

	vector<GameObjectSnapshot> _objectSnapshots;
	vector<ComponentSnapshot> _compSnapshots;
};

