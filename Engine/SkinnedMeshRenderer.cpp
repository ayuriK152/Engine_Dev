#include "pch.h"
#include "SkinnedMeshRenderer.h"

SkinnedMeshRenderer::SkinnedMeshRenderer() : Super()
{

}

SkinnedMeshRenderer::~SkinnedMeshRenderer()
{

}

void SkinnedMeshRenderer::Init()
{
	// 본 데이터가 있는 경우 셰이더 코드의 Structured Buffer
	if (rootBone != nullptr)
	{
		vector<shared_ptr<Transform>> boneTransforms;
		UpdateBoneTransforms(rootBone, boneTransforms);

		for (shared_ptr<Transform>& bt : boneTransforms)
		{
			XMFLOAT4X4 boneOffsetTransform;
			XMStoreFloat4x4(&boneOffsetTransform, XMMatrixInverse(nullptr, XMLoadFloat4x4(&bt->GetWorldMatrix())));
			_boneOffsetTransforms.push_back(boneOffsetTransform);
		}

		UINT64 boneByteSize = sizeof(XMFLOAT4X4) * boneTransforms.size();

		_boneTransformTest = make_unique<UploadBuffer<XMFLOAT4X4>>(boneTransforms.size(), false);

		CreateBoneSRV(boneTransforms);
	}
}

void SkinnedMeshRenderer::Update()
{
	if (rootBone != nullptr)
	{
		vector<shared_ptr<Transform>> boneTransforms;

		UpdateBoneTransforms(rootBone, boneTransforms);

		for (UINT i = 0; i < boneTransforms.size(); ++i)
		{
			XMMATRIX finalMat = XMLoadFloat4x4(&_bones[boneTransforms[i]->GetGameObject()->name]->offsetTransform);
			//if (GetGameObject()->GetComponent<Animator>() != nullptr)
			//{
			//	finalMat = finalMat * GetGameObject()->GetComponent<Animator>()->GetCurrentAnimation()->GetAnimationMatTest(boneTransforms[i]->GetGameObject()->name);
			//}
			finalMat = finalMat * XMLoadFloat4x4(&boneTransforms[i]->GetWorldMatrix());
			XMFLOAT4X4 finalTransform;
			XMStoreFloat4x4(&finalTransform, XMMatrixTranspose(finalMat));
			_boneTransformTest->CopyData(i, finalTransform);
		}
	}
}

void SkinnedMeshRenderer::Render()
{
	auto cmdList = GRAPHIC->GetCommandList().Get();

	if (rootBone != nullptr)
	{
		CD3DX12_GPU_DESCRIPTOR_HANDLE bone(RENDER->GetShaderResourceViewHeap()->GetGPUDescriptorHandleForHeapStart());
		bone.Offset(_boneSrvHeapIndex, GRAPHIC->GetCBVSRVDescriptorSize());

		cmdList->SetGraphicsRootDescriptorTable(ROOT_PARAMETER_BONE_SB, bone);
	}

	Super::Render();
}

void SkinnedMeshRenderer::UpdateBoneTransforms(const shared_ptr<Transform> root, vector<shared_ptr<Transform>>& boneTransforms)
{
	boneTransforms.push_back(root);
	for (auto& t : root->GetChilds())
		UpdateBoneTransforms(t, boneTransforms);
}

void SkinnedMeshRenderer::CreateBoneSRV(vector<shared_ptr<Transform>>& boneTransforms)
{
	_boneSrvHeapIndex = RENDER->GetAndIncreaseSRVHeapIndex();
	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(RENDER->GetShaderResourceViewHeap()->GetCPUDescriptorHandleForHeapStart());
	hDescriptor.Offset(_boneSrvHeapIndex, GRAPHIC->GetCBVSRVDescriptorSize());

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = boneTransforms.size();
	srvDesc.Buffer.StructureByteStride = sizeof(XMFLOAT4X4);
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	GRAPHIC->GetDevice()->CreateShaderResourceView(_boneTransformTest->GetResource(), &srvDesc, hDescriptor);
}
