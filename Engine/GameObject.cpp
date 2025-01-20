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

	UINT cbvIndex = GRAPHIC->GetCurrFrameResourceIndex() * (UINT)GRAPHIC->GetObjects().size() + objCBIndex;
	auto cbvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(GRAPHIC->GetConstantBufferHeap()->GetGPUDescriptorHandleForHeapStart());
	cbvHandle.Offset(cbvIndex, GRAPHIC->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

	GRAPHIC->GetCommandList().Get()->SetGraphicsRootDescriptorTable(0, cbvHandle);

	GRAPHIC->GetCommandList().Get()->DrawIndexedInstanced(indexCount, 1, startIndexLocation, baseVertexLocation, 0);
}
