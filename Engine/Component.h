#pragma once

#pragma region 전방선언
class GameObject;
class Transform;
class Camera;
class Collider;
#pragma endregion

enum class ComponentType
{
	Undefined,
	Transform,
	MeshRenderer,
	SkinnedMeshRenderer,
	Camera,
	Collider,
	Rigidbody,
	Light,
	Animator,
	Script,
	ParticleEmitter
};

class Component : public enable_shared_from_this<Component>
{
public:
	Component(ComponentType type);
	virtual ~Component();

	virtual void Init() { }
	virtual void FixedUpdate() { }
	virtual void Update() { }
	virtual void Render() { }

	virtual void OnCollision(shared_ptr<Collider> other) { }

public:
	shared_ptr<GameObject> GetGameObject();
	shared_ptr<Transform> GetTransform();
	UINT GetID() { return _id; }

	bool IsActive() { return _isActive; }
	void SetActive(bool value) { if (type != ComponentType::Transform) _isActive = value; }

public:
	ComponentType type;
	bool isInitialized = false;

private:
	friend class GameObject;
	void SetGameObject(shared_ptr<GameObject> gameObject) { _gameObject = gameObject; }

private:
	static UINT _count;
	UINT _id;
	weak_ptr<GameObject> _gameObject;
	bool _isActive = true;
};

