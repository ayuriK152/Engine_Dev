#include "pch.h"
#include "FrameResource.h"

FrameResource::FrameResource(UINT objectCount)
{
	ThrowIfFailed(GRAPHIC->GetDevice()->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(cmdListAlloc.GetAddressOf())));

	// �굵 Init���� �Űܾߵ�
	lightSB = make_unique<UploadBuffer<LightConstants>>(DEFAULT_COUNT_LIGHT, false);
	objectCB = make_unique<UploadBuffer<ObjectConstants>>(DEFAULT_COUNT_OBJECT, true);
}

FrameResource::~FrameResource()
{

}

void FrameResource::Init()
{
	CreateLightSRV();
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
	for (auto& o : RENDER->GetObjects())
	{
		if (o->GetFramesDirty() > 0)
		{
			XMMATRIX world = XMLoadFloat4x4(&o->GetTransform()->GetWorldMatrix());

			ObjectConstants objConstants;
			XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));

			objectCB->CopyData(o->objCBIndex, objConstants);

			o->ReleaseFramesDirty();
		}
	}
}

void FrameResource::UpdateLightCB()
{
	bool flag = false;
	LightConstants lightConstants;
	auto lights = RENDER->GetLights();

	if (lights.size() == 0)
		return;

	for (int i = 0; i < lights.size(); i++)
	{
		lightSB->CopyData(i, lights[i]->GetLightConstants());
	}

	//// Global Light
	//if (lights[0]->GetFramesDirty() > 0)
	//{
	//	lightConstants.GlobalLight = lights[0]->GetLightConstants();
	//	lights[0]->ReleaseFramesDirty();
	//	flag = true;
	//}

	//// General Light
	//for (int i = 1; i < lights.size(); i++)
	//{
	//	if (lights[i]->GetFramesDirty() > 0)
	//	{
	//		lightConstants.Lights[i - 1] = lights[i]->GetLightConstants();
	//		lights[i]->ReleaseFramesDirty();
	//		flag = true;
	//	}
	//}

	//if (flag)
	//{
	//	lightSB->CopyData(0, lightConstants);
	//}
}

void FrameResource::CreateLightSRV()
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
	CD3DX12_CPU_DESCRIPTOR_HANDLE heapHandle(RENDER->GetShaderResourceViewHeap()->GetCPUDescriptorHandleForHeapStart());
	heapHandle.Offset(_lightSrvHeapIndex, GRAPHIC->GetCBVSRVDescriptorSize());

	GRAPHIC->GetDevice()->CreateShaderResourceView(lightSB->GetResource(), &srvDesc, heapHandle);
}
