#include "pch.h"
#include "GameObject.h"

GameObject::GameObject()
{
	world = MathHelper::Identity4x4();

	primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	meshName = "";

	objCBIndex = -1;
	indexCount = 0;
	startIndexLocation = 0;
	baseVertexLocation = 0;

	numFramesDirty = GRAPHIC->GetNumFrameResources();
}

GameObject::~GameObject()
{

}

void GameObject::Update()
{
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
