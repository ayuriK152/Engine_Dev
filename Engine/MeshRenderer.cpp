#include "pch.h"
#include "MeshRenderer.h"

MeshRenderer::MeshRenderer() : Super(ComponentType::MeshRenderer)
{

}

MeshRenderer::~MeshRenderer()
{

}

void MeshRenderer::Init()
{

}

void MeshRenderer::Update()
{

}

void MeshRenderer::Render()
{
	GRAPHIC->GetCommandList().Get()->IASetVertexBuffers(0, 1, &_mesh->VertexBufferView());
	GRAPHIC->GetCommandList().Get()->IASetIndexBuffer(&_mesh->IndexBufferView());
	GRAPHIC->GetCommandList().Get()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	CD3DX12_GPU_DESCRIPTOR_HANDLE tex(GRAPHIC->GetShaderResourceViewHeap()->GetGPUDescriptorHandleForHeapStart());
	tex.Offset(_material->diffuseSrvHeapIndex, GRAPHIC->GetCBVSRVDescriptorSize());

	UINT objCBByteSize = DXUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
	UINT matCBByteSize = DXUtil::CalcConstantBufferByteSize(sizeof(MaterialConstants));

	auto objectCB = GRAPHIC->GetCurrFrameResource()->objectCB->GetResource();
	auto matCB = GRAPHIC->GetCurrFrameResource()->materialCB->GetResource();

	D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress() + GetGameObject()->objCBIndex * objCBByteSize;
	D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = matCB->GetGPUVirtualAddress() + GetGameObject()->material->matCBIndex * matCBByteSize;

	//UINT cbvIndex = GRAPHIC->GetCurrFrameResourceIndex() * (UINT)GRAPHIC->GetObjects().size() + objCBIndex;
	//auto cbvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(GRAPHIC->GetConstantBufferHeap()->GetGPUDescriptorHandleForHeapStart());
	//cbvHandle.Offset(cbvIndex, GRAPHIC->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

	GRAPHIC->GetCommandList().Get()->SetGraphicsRootDescriptorTable(0, tex);
	GRAPHIC->GetCommandList().Get()->SetGraphicsRootConstantBufferView(1, objCBAddress);
	GRAPHIC->GetCommandList().Get()->SetGraphicsRootConstantBufferView(3, matCBAddress);

	//GRAPHIC->GetCommandList().Get()->DrawIndexedInstanced(indexCount, 1, startIndexLocation, baseVertexLocation, 0);
	GRAPHIC->GetCommandList().Get()->DrawIndexedInstanced(_mesh->GetIndexCount(), 1, 0, 0, 0);
}
