#pragma once

#pragma region 전방선언
class Geometry;
class Material;
class Component;
class MeshRenderer;
class SkinnedMeshRenderer;
class Transform;
class Camera;
class Rigidbody;
class Animator;
class ParticleEmitter;
class CharacterController;
#pragma endregion


class GameObject : public enable_shared_from_this<GameObject>
{
public:
	GameObject();
	~GameObject();

	void Init();
	void PreUpdate();
	void Update();
	void Render(ID3D12GraphicsCommandList* cmdList, UINT renderState = 0);

	void OnCollisionEnter(shared_ptr<GameObject> other);
	void OnCollision(shared_ptr<GameObject> other);
	void OnCollisionExit(shared_ptr<GameObject> other);

	void OnDestroy();

public:
	void AddComponent(shared_ptr<Component> component);
	shared_ptr<Transform> GetTransform();

	template<typename T>
	shared_ptr<T> GetComponent();

	template<typename T>
	const vector<shared_ptr<T>>& GetAllComponents();

	int GetComponentCount() { return _componentCount; }

	array<vector<shared_ptr<Component>>, COUNT_COMPONENTTYPE> GetAllComponents() { return _components; }

	template<typename T>
	ComponentType GetComponentType();

	int GetComponentTypeIndex(ComponentType type);

	// 인스턴스 ID는 프리팹에 저장하지 않는 정보
	// CB 인덱스와 다르게 숫자 밀려도 상관없음. 그냥 식별할 수 있기만 하면 됨.
	// 그래서 Setter 구현해두지 않음.
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

	void Delete(float time);

public:
	D3D12_PRIMITIVE_TOPOLOGY primitiveType;

	UINT objectID;

	// unordered_map<ComponentType, shared_ptr<Component>> components;

private:
	static int _nextId;
	int _id;
	string _name;
	string _psoName;
	string _tag;

	bool _isInitialized;
	int _numFramesDirty;

	float _deleteTime = 0.0f;
	bool _isDeleteReserved = false;

	int _componentCount = 0;
	array<vector<shared_ptr<Component>>, COUNT_COMPONENTTYPE> _components;
};

template<typename T>
shared_ptr<T> GameObject::GetComponent()
{
	ComponentType componentType = GetComponentType<T>();
	int componentTypeIdx = GetComponentTypeIndex(componentType);

	if (componentTypeIdx == -1) return nullptr;
	if (_components[componentTypeIdx].size() > 0)
		return static_pointer_cast<T>(_components[componentTypeIdx][0]);
	else return nullptr;
}

template<typename T>
ComponentType GameObject::GetComponentType()
{
	if (is_base_of_v<MeshRenderer, T>)
		return ComponentType::MeshRenderer;
	if (is_base_of_v<SkinnedMeshRenderer, T>)
		return ComponentType::SkinnedMeshRenderer;
	if (is_base_of_v<Transform, T>)
		return ComponentType::Transform;
	if (is_base_of_v<Camera, T>)
		return ComponentType::Camera;
	if (is_base_of_v<Rigidbody, T>)
		return ComponentType::Rigidbody;
	if (is_base_of_v<Animator, T>)
		return ComponentType::Animator;
	if (is_base_of_v<Script, T>)
		return ComponentType::Script;
	if (is_base_of_v<ParticleEmitter, T>)
		return ComponentType::ParticleEmitter;
	if (is_base_of_v<CharacterController, T>)
		return ComponentType::CharacterController;

	return ComponentType::Undefined;
}
