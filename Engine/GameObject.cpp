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
	for (auto& c : components) {
		c.second->OnDestroy();
	}

	components.clear();
}

void GameObject::Init()
{
	for (auto& c : components)
	{
		c.second->isInitialized = true;
		c.second->Init();
	}
}

void GameObject::FixedUpdate()
{
	for (auto& c : components)
	{
		c.second->FixedUpdate();
	}
}

void GameObject::Update()
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
	for (auto& c : components)
	{
		// 런타임 중에 추가되는 컴포넌트
		if (!c.second->isInitialized)
		{
			c.second->isInitialized = true;
			c.second->Init();
		}

		if (c.second->type == ComponentType::Collider || 
			c.second->type == ComponentType::ParticleEmitter ||
			c.second->type == ComponentType::Animator)
			continue;
		c.second->Update();
	}
}

void GameObject::Render(ID3D12GraphicsCommandList* cmdList)
{
	for (auto& c : components)
	{
		if (c.second->type == ComponentType::ParticleEmitter)
			continue;
		c.second->Render(cmdList);
	}
}

void GameObject::OnCollision(shared_ptr<Collider> other)
{
	for (auto& c : components)
	{
		c.second->OnCollision(other);
	}
}

void GameObject::AddComponent(shared_ptr<Component> component)
{
	component->SetGameObject(shared_from_this());
	components.insert({component->type, component});
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
