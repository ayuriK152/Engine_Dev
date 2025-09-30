#include "pch.h"
#include "MeshRenderer.h"

MeshRenderer::MeshRenderer() : Super(ComponentType::MeshRenderer)
{

}

MeshRenderer::MeshRenderer(ComponentType type) : Super(type)
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

	UINT objCBByteSize = DXUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));

	auto objectCB = GRAPHIC->GetCurrFrameResource()->objectCB->GetResource();

	D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress() + GetGameObject()->objCBIndex * objCBByteSize;

	cmdList->SetGraphicsRootConstantBufferView(ROOT_PARAM_OBJECT_CB, objCBAddress);


	cmdList->DrawIndexedInstanced(_mesh->GetIndexCount(), 1, 0, 0, 0);
}

void MeshRenderer::SetMesh(shared_ptr<Mesh> mesh)
{
	_mesh = mesh;
	_material = _mesh->GetMaterial();

	// asset parser 임시조치
	if (GRAPHIC->GetDevice() == nullptr)
		return;
	_mesh->CreateBuffer();
}
