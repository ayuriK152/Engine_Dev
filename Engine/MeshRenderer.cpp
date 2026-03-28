#include "pch.h"
#include "MeshRenderer.h"

REGISTER_COMPONENT(MeshRenderer);

MeshRenderer::MeshRenderer() : Super(ComponentType::MeshRenderer)
{

}

MeshRenderer::MeshRenderer(ComponentType type) : Super(type)
{

}

MeshRenderer::~MeshRenderer()
{
	cout << "Released - MeshRenderer:" << _id << "\n";
}

void MeshRenderer::Render(ID3D12GraphicsCommandList* cmdList, UINT renderState)
{
	if (_mesh == nullptr) return;

	switch (renderState) {
	case RENDERSTATE_MAIN:
		if (RENDER->CheckMeshRender(_mesh)) return;
		RENDER->SetMeshRenderCheckValue(_mesh);
		break;

	case RENDERSTATE_SHADOWMAP:
		if (RENDER->CheckMeshShadowRender(_mesh)) return;
		RENDER->SetMeshShadowRenderCheckValue(_mesh);
		break;
	}

	// 버퍼뷰의 직접 접근을 막고 Getter 메소드 정의는 어떤지?
	cmdList->IASetVertexBuffers(0, 1, &_mesh->vertexBufferView);
	cmdList->IASetIndexBuffer(&_mesh->indexBufferView);
	cmdList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT startIndex = RENDER->GetMeshInstanceStartIndex(_mesh);
	cmdList->SetGraphicsRoot32BitConstant(ROOT_PARAM_MESHINFO_C, startIndex, 0);

	cmdList->DrawIndexedInstanced(_mesh->GetIndexCount(), _mesh->GetInstanceCount(), 0, 0, 0);
}

void MeshRenderer::OnDestroy()
{
	cout << "OnDestroy - MeshRenderer:" << _id << "\n";

	if (_mesh != nullptr) {
		if (!RENDER->IsDestructorRunning()) _mesh->DecreaseInstanceCount();
		_mesh.reset();
	}
	if (_material != nullptr)
		_material.reset();
}

void MeshRenderer::LoadXML(XMLElement* compElem)
{
	const char* meshName = compElem->Attribute("MeshName");
	if (meshName != 0) {
		shared_ptr<Mesh> mesh = RESOURCE->Get<Mesh>(Utils::ToWString(meshName));
		if (mesh == nullptr) mesh = RESOURCE->LoadMesh(meshName);
		SetMesh(mesh);
	}

	const char* materialName = compElem->Attribute("Material");
	if (materialName != 0) SetMaterial(RESOURCE->Get<Material>(Utils::ToWString(materialName)));
}

void MeshRenderer::SaveXML(XMLElement* compElem)
{
	compElem->SetAttribute("ComponentType", "MeshRenderer");
	if (_mesh != nullptr) compElem->SetAttribute("MeshName", _mesh->GetName().c_str());
	if (_material != nullptr) compElem->SetAttribute("Material", _material->GetName().c_str());
}

ComponentSnapshot MeshRenderer::CaptureSnapshot()
{
	ComponentSnapshot snapshot;

	snapshot.id = _id;
	snapshot.componentType = "MeshRenderer";

	return snapshot;
}

void MeshRenderer::RestoreSnapshot(ComponentSnapshot snapshot)
{

}

void MeshRenderer::SetMesh(shared_ptr<Mesh> mesh)
{
	if (mesh == nullptr)
		return;

	if (_mesh != nullptr) _mesh->DecreaseInstanceCount();

	_mesh = mesh;
	_mesh->IncreaseInstanceCount();
	_material = _mesh->GetMaterial();

	// asset parser 임시조치
	if (GRAPHIC->GetDevice() == nullptr)
		return;
	_mesh->CreateBuffer();

	GetGameObject()->SetFramesDirty();
}
