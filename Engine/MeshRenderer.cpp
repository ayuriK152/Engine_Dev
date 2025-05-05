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
	// 본 데이터가 있는 경우 셰이더 코드의 Structured Buffer
	if (_mesh->HasBones())
	{
		// id 순으로 정렬해서 업로드
		UINT64 boneByteSize;
		vector<shared_ptr<Bone>> sortedBones;
		vector<XMFLOAT4X4> boneTransforms;

		sortedBones.reserve(_mesh->GetBones().size());
		boneTransforms.reserve(_mesh->GetBones().size());
		boneByteSize = sizeof(XMFLOAT4X4) * _mesh->GetBones().size();

		for (auto& bone : _mesh->GetBones())
			sortedBones.push_back(bone.second);

		sort(sortedBones.begin(), sortedBones.end(), [](shared_ptr<Bone> a, shared_ptr<Bone> b) { return a->id < b->id; });

		for (int i = 0; i < sortedBones.size(); i++)
		{
			XMFLOAT4X4 transform;
			XMStoreFloat4x4(&transform, (XMLoadFloat4x4(&sortedBones[i]->node->transform) * XMLoadFloat4x4(&sortedBones[i]->transform)));
			boneTransforms.push_back(transform);
		}

		LoadBoneData(boneByteSize, boneTransforms);
		CreateBoneSRV(boneTransforms);
	}
}

void MeshRenderer::Update()
{

}

void MeshRenderer::Render()
{
	auto matCB = RENDER->GetMaterialCB()->GetResource();
	auto cmdList = GRAPHIC->GetCommandList().Get();

	if (_mesh->HasBones())
	{
		CD3DX12_GPU_DESCRIPTOR_HANDLE bone(RENDER->GetShaderResourceViewHeap()->GetGPUDescriptorHandleForHeapStart());
		bone.Offset(_boneSrvHeapIndex, GRAPHIC->GetCBVSRVDescriptorSize());

		cmdList->SetGraphicsRootDescriptorTable(ROOT_PARAMETER_BONE_SB, bone);
	}

	// 버퍼뷰의 직접 접근을 막고 Getter 메소드 정의는 어떤지?
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

void MeshRenderer::LoadBoneData(UINT64 boneByteSize, vector<XMFLOAT4X4>& boneTransforms)
{
	ThrowIfFailed(GRAPHIC->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(boneByteSize),
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(_boneTransformBuffer.GetAddressOf())));

	D3D12_SUBRESOURCE_DATA initData = {};
	initData.pData = boneTransforms.data();
	initData.RowPitch = boneByteSize;
	initData.SlicePitch = initData.RowPitch;

	auto device = GRAPHIC->GetDevice().Get();
	auto commandQueue = GRAPHIC->GetCommandQueue().Get();

	ResourceUploadBatch upload(device);
	upload.Begin();
	upload.Upload(_boneTransformBuffer.Get(), 0, &initData, 1);
	upload.Transition(_boneTransformBuffer.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
	auto finish = upload.End(commandQueue);
	finish.wait();
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

	GRAPHIC->GetDevice()->CreateShaderResourceView(_boneTransformBuffer.Get(), &srvDesc, hDescriptor);
}
