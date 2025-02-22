#pragma once

#pragma region 전방선언
class GameObject;
class Transform;
class Camera;
#pragma endregion

enum class ComponentType
{
	Undefined,
	Transform,
	MeshRenderer,
	Camera,
	Collider,
	Rigidbody
};

class Component : public enable_shared_from_this<Component>
{
public:
	Component(ComponentType type);
	virtual ~Component();

	virtual void Init() { }
	virtual void Update() { }
	virtual void Render() { }

	shared_ptr<GameObject> GetGameObject();
	shared_ptr<Transform> GetTransform();

public:
	ComponentType type;

private:
	friend class GameObject;
	void SetGameObject(shared_ptr<GameObject> gameObject) { _gameObject = gameObject; }

private:
	weak_ptr<GameObject> _gameObject;
};

