#include "pch.h"
#include "GameObject.h"

GameObject::GameObject()
{
	world = MathHelper::Identity4x4();

	mesh = nullptr;
	primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	objCBIndex = -1;
	indexCount = 0;
	startIndexLocation = 0;
	baseVertexLocation = 0;

	numFramesDirty = GRAPHIC->GetNumFrameResources();
}

GameObject::~GameObject()
{

}
