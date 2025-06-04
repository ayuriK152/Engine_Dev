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
	// �� �����Ͱ� �ִ� ��� ���̴� �ڵ��� Structured Buffer
	if (rootBone != nullptr)
	{
		vector<shared_ptr<Transform>> boneTransforms;
		UpdateBoneTransforms(rootBone, boneTransforms);

		//vector<shared_ptr<Transform>> boneQueue;
		//boneQueue.push_back(rootBone);
		//while (boneQueue.size() > 0)
		//{
		//	boneTransforms.push_back(boneQueue[0]->GetWorldMatrix());
		//	for (auto& t : boneQueue[0]->GetChilds())
		//		boneQueue.push_back(t);
		//	boneQueue.erase(boneQueue.begin());
		//}

		for (shared_ptr<Transform>& bt: boneTransforms)
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

void MeshRenderer::Update()
{
	if (rootBone != nullptr)
	{
		vector<shared_ptr<Transform>> boneTransforms;

		UpdateBoneTransforms(rootBone, boneTransforms);

		for (UINT i = 0; i < boneTransforms.size(); ++i)
		{
			XMMATRIX finalMat = XMLoadFloat4x4(&boneTransforms[i]->GetWorldMatrix()) * XMLoadFloat4x4(&_bones[boneTransforms[i]->GetGameObject()->name]->offsetTransform);
			XMFLOAT4X4 finalTransform;
			XMStoreFloat4x4(&finalTransform, XMMatrixTranspose(finalMat));
			_boneTransformTest->CopyData(i, finalTransform);
		}
	}
}

void MeshRenderer::Render()
{
	auto matCB = RENDER->GetMaterialCB()->GetResource();
	auto cmdList = GRAPHIC->GetCommandList().Get();

	if (rootBone != nullptr)
	{
		CD3DX12_GPU_DESCRIPTOR_HANDLE bone(RENDER->GetShaderResourceViewHeap()->GetGPUDescriptorHandleForHeapStart());
		bone.Offset(_boneSrvHeapIndex, GRAPHIC->GetCBVSRVDescriptorSize());

		cmdList->SetGraphicsRootDescriptorTable(ROOT_PARAMETER_BONE_SB, bone);
	}

	// ���ۺ��� ���� ������ ���� Getter �޼ҵ� ���Ǵ� ���?
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
}

void MeshRenderer::UpdateBoneTransforms(const shared_ptr<Transform> root, vector<shared_ptr<Transform>>& boneTransforms)
{
	boneTransforms.push_back(root);
	for (auto& t : root->GetChilds())
		UpdateBoneTransforms(t, boneTransforms);
}

void MeshRenderer::CreateBoneSRV(vector<shared_ptr<Transform>>& boneTransforms)
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
