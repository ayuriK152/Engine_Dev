#include "pch.h"
#include "SkinnedMeshRenderer.h"

REGISTER_COMPONENT(SkinnedMeshRenderer);

SkinnedMeshRenderer::SkinnedMeshRenderer() : Super(ComponentType::SkinnedMeshRenderer)
{

}

SkinnedMeshRenderer::~SkinnedMeshRenderer()
{
#ifdef PRINT_DEBUG_CONSOLE_LOG
	cout << "Released - SkinnedMeshRenderer:" << _id << "\n";
#endif
}

void SkinnedMeshRenderer::Init()
{
	// 본 데이터가 있는 경우 셰이더 코드의 Structured Buffer
	if (_rootBone == nullptr) {
		SetRootBone(GetTransform()->GetParent()->GetChild(_rootBoneName));
	}

	_skeleton = SKELETON->GetSkeleton(_rootBone);
	if (_skeleton == nullptr)
		DEBUG->ErrorLog("No Skeleton Exists!");

	GetGameObject()->SetPSOName(PSO_OPAQUE_SKINNED);
}

// 스킨드 메쉬에 대한 인스턴싱은 이뤄지지 않고있음.
void SkinnedMeshRenderer::Render(ID3D12GraphicsCommandList* cmdList, UINT renderState)
{
	if (_rootBone == nullptr) return;

	CD3DX12_GPU_DESCRIPTOR_HANDLE bone(RENDER->GetCommonSRVHeap()->GetGPUDescriptorHandleForHeapStart());
	bone.Offset(_skeleton->GetBoneTransformSRVIndex(), GRAPHIC->GetCBVSRVDescriptorSize());

	cmdList->SetGraphicsRootDescriptorTable(ROOT_PARAM_BONE_SB, bone);

	// 버퍼뷰의 직접 접근을 막고 Getter 메소드 정의는 어떤지?
	cmdList->IASetVertexBuffers(0, 1, &_mesh->vertexBufferView);
	cmdList->IASetIndexBuffer(&_mesh->indexBufferView);
	cmdList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT startIndex = RENDER->GetMeshInstanceStartIndex(_mesh);
	cmdList->SetGraphicsRoot32BitConstant(ROOT_PARAM_MESHINFO_C, startIndex, 0);

	cmdList->DrawIndexedInstanced(_mesh->GetIndexCount(), 1, 0, 0, 0);
}

void SkinnedMeshRenderer::OnDestroy()
{
#ifdef PRINT_DEBUG_CONSOLE_LOG
	cout << "OnDestroy - SkinnedMeshRenderer:" << _id << "\n";
#endif

	if (_rootBone != nullptr)
		_rootBone.reset();
	if (_skeleton != nullptr)
		_skeleton.reset();
	if (_mesh != nullptr)
		_mesh.reset();
	if (_material != nullptr)
		_material.reset();
}

void SkinnedMeshRenderer::LoadXML(XMLElement* compElem)
{
	const char* meshPath = compElem->Attribute("Mesh");
	if (meshPath != 0) SetMesh(RESOURCE->LoadMesh(meshPath));

	const char* rootBoneName = compElem->Attribute("RootBoneName");
	if (rootBoneName != 0) _rootBoneName = rootBoneName;
}

void SkinnedMeshRenderer::SaveXML(XMLElement* compElem)
{
	compElem->SetAttribute("ComponentType", "SkinnedMeshRenderer");

	compElem->SetAttribute("Mesh", _mesh->GetPath().c_str());
	compElem->SetAttribute("RootBoneName", _rootBoneName.c_str());
}

ComponentSnapshot SkinnedMeshRenderer::CaptureSnapshot()
{
	ComponentSnapshot snapshot;

	snapshot.id = _id;
	snapshot.componentType = "SkinnedMeshRenderer";

	return snapshot;
}

void SkinnedMeshRenderer::RestoreSnapshot(ComponentSnapshot snapshot)
{

}

void SkinnedMeshRenderer::SetRootBone(const shared_ptr<Transform> rootBone)
{
	_rootBone = rootBone;
	_rootBoneName = _rootBone->GetGameObject()->GetName();
}
