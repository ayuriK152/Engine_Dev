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
	if (_rootBone != nullptr)
	{
		UINT64 boneByteSize = sizeof(XMFLOAT4X4) * _boneTransforms.size();

		_boneTransformUploadBuffer = make_unique<UploadBuffer<XMFLOAT4X4>>(_boneTransforms.size(), false);

		CreateBoneSRV(_boneTransforms);
	}
}

void SkinnedMeshRenderer::Update()
{
	if (_rootBone != nullptr)
	{
		for (UINT i = 0; i < _boneTransforms.size(); ++i)
		{
			if (_boneTransforms[i]->GetGameObject()->GetFramesDirty() == 0)
				continue;

			// 얘 왜 곱셈 순서 반대로 해야되고 전치행렬 해줘야 작동하는지 이유를 모르겠음
			XMMATRIX finalMat = XMLoadFloat4x4(&_boneTransforms[i]->GetWorldMatrix());
			finalMat = XMLoadFloat4x4(&_bones[_boneTransforms[i]->GetGameObject()->name]->offsetTransform) * finalMat;
			finalMat = XMMatrixTranspose(finalMat);

			XMFLOAT4X4 finalTransform;
			XMStoreFloat4x4(&finalTransform, finalMat);
			_boneTransformUploadBuffer->CopyData(i, finalTransform);
		}
	}
}

void SkinnedMeshRenderer::Render()
{
	auto cmdList = GRAPHIC->GetCommandList().Get();

	if (_rootBone != nullptr)
	{
		CD3DX12_GPU_DESCRIPTOR_HANDLE bone(RENDER->GetShaderResourceViewHeap()->GetGPUDescriptorHandleForHeapStart());
		bone.Offset(_boneSrvHeapIndex, GRAPHIC->GetCBVSRVDescriptorSize());

		cmdList->SetGraphicsRootDescriptorTable(ROOT_PARAMETER_BONE_SB, bone);
	}

	Super::Render();
}

void SkinnedMeshRenderer::UpdateBoneTransforms(const shared_ptr<Transform> root)
{
	_boneTransforms.push_back(root);
	for (auto& t : root->GetChilds())
		UpdateBoneTransforms(t);
}

void SkinnedMeshRenderer::SetRootBone(const shared_ptr<Transform> rootBone)
{
	_rootBone = rootBone;

	UpdateBoneTransforms(_rootBone);
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

	GRAPHIC->GetDevice()->CreateShaderResourceView(_boneTransformUploadBuffer->GetResource(), &srvDesc, hDescriptor);
}
