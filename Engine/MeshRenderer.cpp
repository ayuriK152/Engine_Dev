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
	if (RENDER->IsMeshRendered(_mesh))
		return;
	RENDER->SetMeshRenderCheckValue(_mesh);

	auto cmdList = GRAPHIC->GetCommandList().Get();

	// ���ۺ��� ���� ������ ���� Getter �޼ҵ� ���Ǵ� ���?
	cmdList->IASetVertexBuffers(0, 1, &_mesh->vertexBufferView);
	cmdList->IASetIndexBuffer(&_mesh->indexBufferView);
	cmdList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT startIndex = RENDER->GetMeshInstanceStartIndex(_mesh);
	cmdList->SetGraphicsRoot32BitConstant(ROOT_PARAM_MESHINFO_CB, startIndex, 0);

	cmdList->DrawIndexedInstanced(_mesh->GetIndexCount(), _mesh->GetInstanceCount(), 0, 0, 0);
}

void MeshRenderer::SetMesh(shared_ptr<Mesh> mesh)
{
	if (_mesh != nullptr) _mesh->DecreaseInstanceCount();

	_mesh = mesh;
	_mesh->IncreaseInstanceCount();
	_material = _mesh->GetMaterial();

	// asset parser �ӽ���ġ
	if (GRAPHIC->GetDevice() == nullptr)
		return;
	_mesh->CreateBuffer();
}
