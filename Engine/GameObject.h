#pragma once

#pragma region 전방선언
class Geometry;
class Material;
class Component;
class MeshRenderer;
class Transform;
class Camera;
class Rigidbody;
class Collider;
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

	void AddComponent(shared_ptr<Component> component);
	shared_ptr<Transform> GetTransform();

	template<typename T>
	shared_ptr<T> GetComponent();

	template<typename T>
	ComponentType GetComponentType();

public:
	//XMFLOAT4X4 world;

	D3D12_PRIMITIVE_TOPOLOGY primitiveType;

	string meshName;
	string psoName;

	UINT objCBIndex;
	UINT indexCount;
	UINT startIndexLocation;
	int baseVertexLocation;

	int numFramesDirty;

	map<ComponentType, shared_ptr<Component>> components;

private:
	bool _isInitialized;
};

template<typename T>
shared_ptr<T> GameObject::GetComponent()
{
	if (components.contains(GetComponentType<T>()))
		return static_pointer_cast<T>(components[GetComponentType<T>()]);
	else
		return nullptr;
}

template<typename T>
ComponentType GameObject::GetComponentType()
{
	if (is_same_v<T, MeshRenderer>)
		return ComponentType::MeshRenderer;
	if (is_same_v<T, Transform>)
		return ComponentType::Transform;
	if (is_same_v<T, Camera>)
		return ComponentType::Camera;
	if (is_same_v<T, Rigidbody>)
		return ComponentType::Rigidbody;
	if (is_same_v<T, Collider>)
		return ComponentType::Collider;

	return ComponentType::Undefined;
}
