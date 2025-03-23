#include "pch.h"
#include "GameObject.h"

GameObject::GameObject()
{
	primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	meshName = "";
	psoName = PSO_OPAQUE_SOLID;

	objCBIndex = -1;
	indexCount = 0;
	startIndexLocation = 0;
	baseVertexLocation = 0;

	numFramesDirty = GRAPHIC->GetNumFrameResources();
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
