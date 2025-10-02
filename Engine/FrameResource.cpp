#include "pch.h"
#include "FrameResource.h"

FrameResource::FrameResource(UINT objectCount)
{
	ThrowIfFailed(GRAPHIC->GetDevice()->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(cmdListAlloc.GetAddressOf())));

	// ¾êµµ InitÀ¸·Î ¿Å°Ü¾ßµÊ
	lightSB = make_unique<UploadBuffer<LightConstants>>(DEFAULT_COUNT_LIGHT, false);
	instanceSB = make_unique<UploadBuffer<InstanceConstants>>(DEFAULT_INSTANCE_COUNT, false);
}

FrameResource::~FrameResource()
{

}

void FrameResource::Init()
{
	BuildInstanceBufferSRV();
	BuildLightBufferSRV();
}

void FrameResource::Update()
{
	if (!_isInitialized)
	{
		Init();
		_isInitialized = true;
	}
	UpdateObjectCB();
	UpdateLightCB();
}

void FrameResource::UpdateObjectCB()
{
	unordered_map<shared_ptr<Mesh>, int> instanceIndexStacks;
	for (auto& o : RENDER->GetObjects())
	{
		shared_ptr<MeshRenderer> meshRenderer = o->GetComponent<MeshRenderer>();
		if (meshRenderer == nullptr) meshRenderer = o->GetComponent<SkinnedMeshRenderer>();
		if (meshRenderer == nullptr) continue;

		int instanceIndex = RENDER->GetMeshInstanceStartIndex(meshRenderer->GetMesh());
		if (instanceIndexStacks.contains(meshRenderer->GetMesh()))
			instanceIndex += instanceIndexStacks[meshRenderer->GetMesh()]++;
		else
			instanceIndexStacks[meshRenderer->GetMesh()] = 1;

		if (o->GetFramesDirty() > 0)
		{
			o->ReleaseFramesDirty();

			InstanceConstants instanceConstants;

			instanceConstants.MaterialIndex = meshRenderer->GetMaterial()->matSBIndex;

			XMMATRIX world = XMLoadFloat4x4(&o->GetTransform()->GetWorldMatrix());
			XMStoreFloat4x4(&instanceConstants.World, XMMatrixTranspose(world));
			XMStoreFloat4x4(&instanceConstants.WorldInv, XMMatrixTranspose(XMMatrixInverse(nullptr, world)));

			instanceSB->CopyData(instanceIndex, instanceConstants);
		}
	}
}

void FrameResource::UpdateLightCB()
{
	auto lights = RENDER->GetLights();

	if (lights.size() == 0)
		return;

	for (int i = 0; i < lights.size(); i++)
	{
		if (lights[i]->GetFramesDirty() <= 0)
			continue;
		
		lightSB->CopyData(i, lights[i]->GetLightConstants());
	}
}

void FrameResource::BuildInstanceBufferSRV()
{
	_instanceSrvHeapIndex = RENDER->GetAndIncreaseSRVHeapIndex();
	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(RENDER->GetCommonSRVHeap()->GetCPUDescriptorHandleForHeapStart());
	hDescriptor.Offset(_instanceSrvHeapIndex, GRAPHIC->GetCBVSRVDescriptorSize());

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = DEFAULT_INSTANCE_COUNT;
	srvDesc.Buffer.StructureByteStride = sizeof(InstanceConstants);
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	GRAPHIC->GetDevice()->CreateShaderResourceView(instanceSB->GetResource(), &srvDesc, hDescriptor);
}

void FrameResource::BuildLightBufferSRV()
{
	auto lights = RENDER->GetLights();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = DEFAULT_COUNT_LIGHT;
	srvDesc.Buffer.StructureByteStride = sizeof(LightConstants);
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	_lightSrvHeapIndex = RENDER->GetAndIncreaseSRVHeapIndex();
	CD3DX12_CPU_DESCRIPTOR_HANDLE heapHandle(RENDER->GetCommonSRVHeap()->GetCPUDescriptorHandleForHeapStart());
	heapHandle.Offset(_lightSrvHeapIndex, GRAPHIC->GetCBVSRVDescriptorSize());

	GRAPHIC->GetDevice()->CreateShaderResourceView(lightSB->GetResource(), &srvDesc, heapHandle);
}
