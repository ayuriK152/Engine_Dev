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
		vector<XMFLOAT4X4> boneTransforms;

		vector<shared_ptr<Transform>> boneQueue;
		boneQueue.push_back(rootBone);
		while (boneQueue.size() > 0)
		{
			boneTransforms.push_back(boneQueue[0]->GetWorldMatrix());
			for (auto& t : boneQueue[0]->GetChilds())
				boneQueue.push_back(t);
			boneQueue.erase(boneQueue.begin());
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
		vector<XMFLOAT4X4> boneTransforms;

		vector<shared_ptr<Transform>> boneQueue;
		boneQueue.push_back(rootBone);
		while (boneQueue.size() > 0)
		{
			boneTransforms.push_back(boneQueue[0]->GetWorldMatrix());
			for (auto& t : boneQueue[0]->GetChilds())
				boneQueue.push_back(t);
			boneQueue.erase(boneQueue.begin());
		}

		for (UINT i = 0; i < boneTransforms.size(); ++i)
		{
			_boneTransformTest->CopyData(i, boneTransforms[i]);
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

void MeshRenderer::CreateBoneSRV(vector<XMFLOAT4X4>& boneTransforms)
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
