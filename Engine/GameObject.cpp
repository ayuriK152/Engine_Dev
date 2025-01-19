#include "pch.h"
#include "GameObject.h"

GameObject::GameObject()
{
	world = MathHelper::Identity4x4();

	geometry = nullptr;
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

void GameObject::Render()
{
	GRAPHIC->GetCommandList().Get()->IASetVertexBuffers(0, 1, &geometry->VertexBufferView());
	GRAPHIC->GetCommandList().Get()->IASetIndexBuffer(&geometry->IndexBufferView());
	GRAPHIC->GetCommandList().Get()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	GRAPHIC->GetCommandList().Get()->SetGraphicsRootDescriptorTable(0, GRAPHIC->GetConstantBufferHeap()->GetGPUDescriptorHandleForHeapStart());

	GRAPHIC->GetCommandList().Get()->DrawIndexedInstanced(geometry->drawArgs[meshName].indexCount,
		1, 0, 0, 0);
}
