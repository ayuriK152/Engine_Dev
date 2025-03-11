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
	auto submeshes = _mesh->GetSubmeshes();
	auto matCB = RENDER->GetMaterialCB()->GetResource();
	for (auto& submesh : submeshes)
	{
		GRAPHIC->GetCommandList().Get()->IASetVertexBuffers(0, 1, &submesh->VertexBufferView());
		GRAPHIC->GetCommandList().Get()->IASetIndexBuffer(&submesh->IndexBufferView());
		GRAPHIC->GetCommandList().Get()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		CD3DX12_GPU_DESCRIPTOR_HANDLE tex(RENDER->GetShaderResourceViewHeap()->GetGPUDescriptorHandleForHeapStart());
		tex.Offset(submesh->GetMaterial()->diffuseSrvHeapIndex, GRAPHIC->GetCBVSRVDescriptorSize());

		UINT objCBByteSize = DXUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
		UINT matCBByteSize = DXUtil::CalcConstantBufferByteSize(sizeof(MaterialConstants));

		auto objectCB = GRAPHIC->GetCurrFrameResource()->objectCB->GetResource();

		D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress() + GetGameObject()->objCBIndex * objCBByteSize;
		D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = matCB->GetGPUVirtualAddress() + submesh->GetMaterial()->matCBIndex * matCBByteSize;

		GRAPHIC->GetCommandList().Get()->SetGraphicsRootDescriptorTable(0, tex);
		GRAPHIC->GetCommandList().Get()->SetGraphicsRootConstantBufferView(1, objCBAddress);
		GRAPHIC->GetCommandList().Get()->SetGraphicsRootConstantBufferView(3, matCBAddress);

		GRAPHIC->GetCommandList().Get()->DrawIndexedInstanced(submesh->GetIndexCount(), 1, 0, 0, 0);
	}
}
