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
	// �� �����Ͱ� �ִ� ��� ���̴� �ڵ��� Structured Buffer
	if (_rootBone != nullptr)
	{
		_boneTransformUploadBuffer = make_unique<UploadBuffer<XMFLOAT4X4>>(_boneTransforms.size(), false);
		CreateBoneSRV(_boneTransforms);
	}

	GetGameObject()->SetPSONameIncludeChilds(PSO_OPAQUE_SKINNED);
}

void SkinnedMeshRenderer::Update()
{
	if (_rootBone != nullptr)
	{
		for (UINT i = 0; i < _boneTransforms.size(); ++i)
		{
			if (_boneTransforms[i]->GetGameObject()->GetFramesDirty() == 0)
				continue;

			XMMATRIX finalMat = XMLoadFloat4x4(&_boneTransforms[i]->GetWorldMatrix());
			finalMat = XMLoadFloat4x4(&_boneOffsets[i]) * finalMat;
			finalMat = XMMatrixTranspose(finalMat);

			XMStoreFloat4x4(&_finalMatrices[i], finalMat);
		}
		_boneTransformUploadBuffer->CopyData(_finalMatrices, _boneTransforms.size());
	}
}

void SkinnedMeshRenderer::Render()
{
	auto cmdList = GRAPHIC->GetCommandList().Get();

	if (_rootBone != nullptr)
	{
		CD3DX12_GPU_DESCRIPTOR_HANDLE bone(RENDER->GetCommonSRVHeap()->GetGPUDescriptorHandleForHeapStart());
		bone.Offset(_boneTransformSrvIndex, GRAPHIC->GetCBVSRVDescriptorSize());

		cmdList->SetGraphicsRootDescriptorTable(ROOT_PARAM_BONE_SB, bone);
	}

	Super::Render();
}

void SkinnedMeshRenderer::UpdateBoneTransforms(const shared_ptr<Transform> root)
{
	_boneTransforms.push_back(root);
	for (auto& t : root->GetChilds())
		UpdateBoneTransforms(t);
}

void SkinnedMeshRenderer::SetBoneData(const map<string, Bone>& bones)
{
	for (auto b : bones)
		_boneOffsets[b.second.id] = b.second.offsetTransform;
}

void SkinnedMeshRenderer::SetRootBone(const shared_ptr<Transform> rootBone)
{
	_rootBone = rootBone;

	_boneTransforms.clear();
	UpdateBoneTransforms(_rootBone);
}

void SkinnedMeshRenderer::CreateBoneSRV(vector<shared_ptr<Transform>>& boneTransforms)
{
	// BoneTransform Buffer
	_boneTransformSrvIndex = RENDER->GetAndIncreaseSRVHeapIndex();
	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(RENDER->GetCommonSRVHeap()->GetCPUDescriptorHandleForHeapStart());
	hDescriptor.Offset(_boneTransformSrvIndex, GRAPHIC->GetCBVSRVDescriptorSize());

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
