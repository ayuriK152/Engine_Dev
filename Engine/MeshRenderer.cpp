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
	auto matCB = RENDER->GetMaterialCB()->GetResource();
	auto cmdList = GRAPHIC->GetCommandList().Get();

	// 버퍼뷰의 직접 접근을 막고 Getter 메소드 정의는 어떤지?
	cmdList->IASetVertexBuffers(0, 1, &_mesh->vertexBufferView);
	cmdList->IASetIndexBuffer(&_mesh->indexBufferView);
	cmdList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	CD3DX12_GPU_DESCRIPTOR_HANDLE tex(RENDER->GetShaderResourceViewHeap()->GetGPUDescriptorHandleForHeapStart());
	tex.Offset(_material->diffuseSrvHeapIndex, GRAPHIC->GetCBVSRVDescriptorSize());

	UINT objCBByteSize = DXUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
	UINT matCBByteSize = DXUtil::CalcConstantBufferByteSize(sizeof(MaterialConstants));

	auto objectCB = GRAPHIC->GetCurrFrameResource()->objectCB->GetResource();

	D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress() + GetGameObject()->objCBIndex * objCBByteSize;
	D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = matCB->GetGPUVirtualAddress() + _material->matCBIndex * matCBByteSize;

	if (RENDER->GetSkyboxTexSRVHeapIndex() >= 0)
	{
		CD3DX12_GPU_DESCRIPTOR_HANDLE skyboxTex(RENDER->GetShaderResourceViewHeap()->GetGPUDescriptorHandleForHeapStart());
		skyboxTex.Offset(RENDER->GetSkyboxTexSRVHeapIndex(), GRAPHIC->GetCBVSRVDescriptorSize());
		cmdList->SetGraphicsRootDescriptorTable(ROOT_PARAMETER_SKYBOX_SR, skyboxTex);
	}
	cmdList->SetGraphicsRootDescriptorTable(ROOT_PARAMETER_TEXTURE_SR, tex);
	cmdList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_OBJECT_CB, objCBAddress);
	cmdList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_MATERIAL_CB, matCBAddress);


	cmdList->DrawIndexedInstanced(_mesh->GetIndexCount(), 1, 0, 0, 0);
}

void MeshRenderer::SetMesh(shared_ptr<Mesh> mesh)
{
	_mesh = mesh;
	_material = _mesh->GetMaterial();
	_mesh->CreateBuffer();
}
