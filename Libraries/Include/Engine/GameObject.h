#pragma once

#pragma region ���漱��
class Geometry;
class Material;
class Component;
class MeshRenderer;
class SkinnedMeshRenderer;
class Transform;
class Camera;
class Rigidbody;
class Collider;
class Animator;
#pragma endregion


class GameObject : public enable_shared_from_this<GameObject>
{
public:
	GameObject();
	~GameObject();

	void Init();
	void FixedUpdate();
	void Update();
	void Render();

	void OnCollision(shared_ptr<Collider> other);

public:
	void AddComponent(shared_ptr<Component> component);
	shared_ptr<Transform> GetTransform();

	template<typename T>
	shared_ptr<T> GetComponent();

	map<ComponentType, shared_ptr<Component>> GetComponents() { return components; }

	template<typename T>
	ComponentType GetComponentType();

	// �ν��Ͻ� ID�� �����տ� �������� �ʴ� ����
	// CB �ε����� �ٸ��� ���� �з��� �������. �׳� �ĺ��� �� �ֱ⸸ �ϸ� ��.
	// �׷��� Setter �����ص��� ����.
	int GetId() { return _id; }

	string GetName() { return _name; };
	void SetName(const string& name) { _name = name; }

	string GetPSOName() { return _psoName; }
	void SetPSOName(const string& name) { _psoName = name; }
	void SetPSONameIncludeChilds(const string& name);

	string GetTag() { return _tag; }
	void SetTag(const string& tag) { _tag = tag; }

	void SetFramesDirty();
	int GetFramesDirty() { return _numFramesDirty; }
	void ReleaseFramesDirty() { _numFramesDirty -= 1; }

public:
	D3D12_PRIMITIVE_TOPOLOGY primitiveType;

	UINT objCBIndex;

	map<ComponentType, shared_ptr<Component>> components;

private:
	static int _nextId;
	int _id;
	string _name;
	string _psoName;
	string _tag;

	bool _isInitialized;
	int _numFramesDirty;
};

template<typename T>
shared_ptr<T> GameObject::GetComponent()
{
	ComponentType componentType = GetComponentType<T>();
	if (components.contains(componentType))
		return static_pointer_cast<T>(components[componentType]);
	else if (componentType == ComponentType::SkinnedMeshRenderer)
		return static_pointer_cast<T>(components[ComponentType::MeshRenderer]);
	else
		return nullptr;
}

template<typename T>
ComponentType GameObject::GetComponentType()
{
	if (is_same_v<T, MeshRenderer>)
		return ComponentType::MeshRenderer;
	if (is_same_v<T, SkinnedMeshRenderer>)
		return ComponentType::SkinnedMeshRenderer;
	if (is_same_v<T, Transform>)
		return ComponentType::Transform;
	if (is_same_v<T, Camera>)
		return ComponentType::Camera;
	if (is_same_v<T, Rigidbody>)
		return ComponentType::Rigidbody;
	if (is_same_v<T, Collider>)
		return ComponentType::Collider;
	if (is_same_v<T, Animator>)
		return ComponentType::Animator;
	if (is_same_v<T, Script>)
		return ComponentType::Script;

	return ComponentType::Undefined;
}
