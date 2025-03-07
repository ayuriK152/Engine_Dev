#include "pch.h"
#include "MeshRenderer.h"

MeshRenderer::MeshRenderer() : Super(ComponentType::MeshRenderer)
{
	_materialCB = make_unique<UploadBuffer<MaterialConstants>>(GRAPHIC->GetDevice().Get(), (UINT)1, true);
}

MeshRenderer::~MeshRenderer()
{

}

void MeshRenderer::Init()
{
	
}

void MeshRenderer::Update()
{
	if (_material->numFramesDirty > 0)
	{
		XMMATRIX matTransform = XMLoadFloat4x4(&_material->matTransform);

		MaterialConstants matConstants;
		matConstants.diffuse = _material->diffuse;
		matConstants.fresnel = _material->fresnel;
		matConstants.roughness = _material->roughness;
		XMStoreFloat4x4(&matConstants.matTransform, XMMatrixTranspose(matTransform));

		_materialCB->CopyData(_material->matCBIndex, matConstants);

		_material->numFramesDirty--;
	}
}

void MeshRenderer::Render()
{
	auto submeshes = _mesh->GetSubmeshes();
	for (auto& submesh : submeshes)
	{
		GRAPHIC->GetCommandList().Get()->IASetVertexBuffers(0, 1, &submesh->VertexBufferView());
		GRAPHIC->GetCommandList().Get()->IASetIndexBuffer(&submesh->IndexBufferView());
		GRAPHIC->GetCommandList().Get()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		CD3DX12_GPU_DESCRIPTOR_HANDLE tex(RENDER->GetShaderResourceViewHeap()->GetGPUDescriptorHandleForHeapStart());
		tex.Offset(_material->diffuseSrvHeapIndex, GRAPHIC->GetCBVSRVDescriptorSize());

		UINT objCBByteSize = DXUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
		UINT matCBByteSize = DXUtil::CalcConstantBufferByteSize(sizeof(MaterialConstants));

		auto objectCB = GRAPHIC->GetCurrFrameResource()->objectCB->GetResource();
		auto matCB = _materialCB->GetResource();

		D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress() + GetGameObject()->objCBIndex * objCBByteSize;
		D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = matCB->GetGPUVirtualAddress() + _material->matCBIndex * matCBByteSize;

		GRAPHIC->GetCommandList().Get()->SetGraphicsRootDescriptorTable(0, tex);
		GRAPHIC->GetCommandList().Get()->SetGraphicsRootConstantBufferView(1, objCBAddress);
		GRAPHIC->GetCommandList().Get()->SetGraphicsRootConstantBufferView(3, matCBAddress);

		GRAPHIC->GetCommandList().Get()->DrawIndexedInstanced(submesh->GetIndexCount(), 1, 0, 0, 0);
	}
}
