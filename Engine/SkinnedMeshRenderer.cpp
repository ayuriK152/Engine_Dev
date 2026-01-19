#include "pch.h"
#include "SkinnedMeshRenderer.h"

SkinnedMeshRenderer::SkinnedMeshRenderer() : Super(ComponentType::SkinnedMeshRenderer)
{

}

SkinnedMeshRenderer::~SkinnedMeshRenderer()
{

}

void SkinnedMeshRenderer::Init()
{
	// 본 데이터가 있는 경우 셰이더 코드의 Structured Buffer
	if (_rootBone != nullptr)
	{
		_skeleton = SKELETON->GetSkeleton(_rootBone);
		if (_skeleton == nullptr)
			_skeleton = SKELETON->LoadSkeleton(_boneFileName, _rootBone);
	}

	GetGameObject()->SetPSONameIncludeChilds(PSO_OPAQUE_SKINNED);
}

void SkinnedMeshRenderer::Render()
{
	auto cmdList = GRAPHIC->GetCommandList().Get();

	if (_rootBone != nullptr)
	{
		CD3DX12_GPU_DESCRIPTOR_HANDLE bone(RENDER->GetCommonSRVHeap()->GetGPUDescriptorHandleForHeapStart());
		bone.Offset(_skeleton->GetBoneTransformSRVIndex(), GRAPHIC->GetCBVSRVDescriptorSize());
		
		cmdList->SetGraphicsRootDescriptorTable(ROOT_PARAM_BONE_SB, bone);
	}

	Super::Render();
}

void SkinnedMeshRenderer::SetRootBone(const shared_ptr<Transform> rootBone)
{
	_rootBone = rootBone;
}
