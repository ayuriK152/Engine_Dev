#include "pch.h"
#include "GameObject.h"

GameObject::GameObject()
{
	world = MathHelper::Identity4x4();

	geometry = nullptr;
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

void GameObject::Render()
{
	ID3D12GraphicsCommandList* cmdList = GRAPHIC->GetCommandList();

	cmdList->IASetVertexBuffers(0, 1, &geometry->VertexBufferView());
	cmdList->IASetIndexBuffer(&geometry->IndexBufferView());
	cmdList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	cmdList->SetGraphicsRootDescriptorTable(0, GRAPHIC->GetConstantBufferHeap()->GetGPUDescriptorHandleForHeapStart());

	cmdList->DrawIndexedInstanced(geometry->drawArgs["box"].indexCount,
		1, 0, 0, 0);
}
