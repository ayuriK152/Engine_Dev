#include "pch.h"
#include "GameObject.h"

int GameObject::_nextId = 0;

GameObject::GameObject()
{
	primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	_id = _nextId++;

	_name = "GameObject";
	_psoName = PSO_OPAQUE_SOLID;
	_tag = "Default";

	_numFramesDirty = RENDER->GetNumFrameResources();
	_isInitialized = false;
}

GameObject::~GameObject()
{
	for (auto& componentVec : _components) {
		if (componentVec.size() == 0) continue;

		for (auto& c : componentVec)
			c->OnDestroy();
		componentVec.clear();
	}
}

void GameObject::Init()
{
	for (auto& componentVec : _components) {
		if (componentVec.size() == 0) continue;

		for (auto& c : componentVec) {
			c->isInitialized = true;
			c->Init();
		}
	}
}

void GameObject::PreUpdate()
{
	if (_isDeleteReserved) {
		_deleteTime -= TIME->DeltaTime();
		if (_deleteTime <= 0.0f) {
			RENDER->DeleteGameobject(shared_from_this());
			return;
		}
	}

	if (!_isInitialized) {
		_isInitialized = true;
		Init();
	}

	for (auto& componentVec : _components) {
		if (componentVec.size() == 0) continue;

		for (auto& c : componentVec) {
			if (!c->isInitialized) {
				c->isInitialized = true;
				c->Init();
			}
			c->PreUpdate();
		}
	}
}

void GameObject::Update()
{
	for (auto& componentVec : _components) {
		if (componentVec.size() == 0) continue;

		for (auto& c : componentVec) {
			// 런타임 중에 추가되는 컴포넌트

			if (c->type == ComponentType::ParticleEmitter ||
				c->type == ComponentType::Animator)
				continue;
			c->Update();
		}
	}
}

void GameObject::Render(ID3D12GraphicsCommandList* cmdList)
{
	for (auto& componentVec : _components) {
		if (componentVec.size() == 0) continue;

		for (auto& c : componentVec) {
			if (c->type == ComponentType::ParticleEmitter)
				continue;
			c->Render(cmdList);
		}
	}
}

void GameObject::OnCollisionEnter(shared_ptr<GameObject> other)
{
	for (auto& componentVec : _components) {
		if (componentVec.size() == 0) continue;

		for (auto& c : componentVec) {
			c->OnCollisionEnter(other);
		}
	}
}

void GameObject::OnCollision(shared_ptr<GameObject> other)
{
	for (auto& componentVec : _components) {
		if (componentVec.size() == 0) continue;

		for (auto& c : componentVec) {
			c->OnCollision(other);
		}
	}
}

void GameObject::OnCollisionExit(shared_ptr<GameObject> other)
{
	for (auto& componentVec : _components) {
		if (componentVec.size() == 0) continue;

		for (auto& c : componentVec) {
			c->OnCollisionExit(other);
		}
	}
}

void GameObject::AddComponent(shared_ptr<Component> component)
{
	int componentTypeIdx = GetComponentTypeIndex(component->type);
	if (componentTypeIdx == -1) return;
	
	++_componentCount;
	component->SetGameObject(shared_from_this());
	_components[componentTypeIdx].push_back(component);
}

shared_ptr<Transform> GameObject::GetTransform()
{
	auto transform = GetComponent<Transform>();
	if (transform == nullptr) {
		transform = make_shared<Transform>();
		AddComponent(transform);
	}
	return transform;
}

int GameObject::GetComponentTypeIndex(ComponentType type)
{
	if (type == ComponentType::Undefined)			return -1;
	if (type == ComponentType::Transform)			return 0;
	if (type == ComponentType::MeshRenderer)		return 1;
	if (type == ComponentType::SkinnedMeshRenderer) return 2;
	if (type == ComponentType::Camera)				return 3;
	if (type == ComponentType::Script)				return 4;
	if (type == ComponentType::Light)				return 5;
	if (type == ComponentType::Animator)			return 6;
	if (type == ComponentType::Rigidbody)			return 7;
	if (type == ComponentType::ParticleEmitter)		return 8;
	if (type == ComponentType::CharacterController) return 9;
}

void GameObject::SetPSONameIncludeChilds(const string& name)
{
	RENDER->UpdateObjectPSO(shared_from_this(), name);
	_psoName = name;
}

void GameObject::SetFramesDirty()
{
	_numFramesDirty = RENDER->GetNumFrameResources();
}

void GameObject::Delete(float time)
{
	_isDeleteReserved = true;
	_deleteTime = time;
}
