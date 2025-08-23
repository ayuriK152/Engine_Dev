#include "pch.h"
#include "GameObject.h"

int GameObject::_nextId = 0;

GameObject::GameObject()
{
	primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	_id = _nextId++;

	_name = "GameObject";
	_psoName = PSO_OPAQUE_SOLID;

	objCBIndex = -1;

	_numFramesDirty = GRAPHIC->GetNumFrameResources();
	_isInitialized = false;
}

GameObject::~GameObject()
{

}

void GameObject::Init()
{
	for (auto& c : components)
	{
		c.second->Init();
		c.second->isInitialized = true;
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
	if (!_isInitialized) {
		Init();
		_isInitialized = true;
	}
	for (auto& c : components)
	{
		if (!c.second->isInitialized)
		{
			c.second->Init();
			c.second->isInitialized = true;
		}
		if (c.second->type == ComponentType::Collider)
			continue;
		c.second->Update();
	}
}

void GameObject::Render()
{
	for (auto& c : components)
	{
		c.second->Render();
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
	_numFramesDirty = GRAPHIC->GetNumFrameResources();
}
