#include "pch.h"
#include "GameObject.h"

GameObject::GameObject()
{
	world = MathHelper::Identity4x4();

	material = nullptr;
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
	for (auto& c : components)
	{
		c.second->Render();
	}
	/*
	GRAPHIC->GetCommandList().Get()->IASetVertexBuffers(0, 1, &geometry->VertexBufferView());
	GRAPHIC->GetCommandList().Get()->IASetIndexBuffer(&geometry->IndexBufferView());
	GRAPHIC->GetCommandList().Get()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	CD3DX12_GPU_DESCRIPTOR_HANDLE tex(GRAPHIC->GetShaderResourceViewHeap()->GetGPUDescriptorHandleForHeapStart());
	tex.Offset(material->diffuseSrvHeapIndex, GRAPHIC->GetCBVSRVDescriptorSize());

	UINT objCBByteSize = DXUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
	UINT matCBByteSize = DXUtil::CalcConstantBufferByteSize(sizeof(MaterialConstants));

	auto objectCB = GRAPHIC->GetCurrFrameResource()->objectCB->GetResource();
	auto matCB = GRAPHIC->GetCurrFrameResource()->materialCB->GetResource();

	D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress() + objCBIndex * objCBByteSize;
	D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = matCB->GetGPUVirtualAddress() + material->matCBIndex * matCBByteSize;

	//UINT cbvIndex = GRAPHIC->GetCurrFrameResourceIndex() * (UINT)GRAPHIC->GetObjects().size() + objCBIndex;
	//auto cbvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(GRAPHIC->GetConstantBufferHeap()->GetGPUDescriptorHandleForHeapStart());
	//cbvHandle.Offset(cbvIndex, GRAPHIC->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

	GRAPHIC->GetCommandList().Get()->SetGraphicsRootDescriptorTable(0, tex);
	GRAPHIC->GetCommandList().Get()->SetGraphicsRootConstantBufferView(1, objCBAddress);
	GRAPHIC->GetCommandList().Get()->SetGraphicsRootConstantBufferView(3, matCBAddress);

	//GRAPHIC->GetCommandList().Get()->DrawIndexedInstanced(indexCount, 1, startIndexLocation, baseVertexLocation, 0);
	GRAPHIC->GetCommandList().Get()->DrawIndexedInstanced(indexCount, 1, 0, 0, 0);
	*/
}

void GameObject::AddComponent(shared_ptr<Component> component)
{
	component->SetGameObject(shared_from_this());
	components.insert({component->type, component});
}
