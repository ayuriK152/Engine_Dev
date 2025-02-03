#pragma once

#pragma region 전방선언
class GameObject;
#pragma endregion

enum class ComponentType
{
	Undefined,
	Transform,
	MeshRenderer
};

class Component
{
public:
	Component(ComponentType type);
	virtual ~Component();

	virtual void Init() { }
	virtual void Update() { }
	virtual void Render() { }

	shared_ptr<GameObject> GetGameObject();

public:
	ComponentType type;

private:
	friend class GameObject;
	void SetGameObject(shared_ptr<GameObject> gameObject) { _gameObject = gameObject; }

private:
	weak_ptr<GameObject> _gameObject;
};

