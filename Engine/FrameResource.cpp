#include "pch.h"
#include "FrameResource.h"

FrameResource::FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount)
{
	ThrowIfFailed(device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(cmdListAlloc.GetAddressOf())));

	lightCB = make_unique<UploadBuffer<LightGatherConstants>>(device, passCount, true);
	if (objectCount > 0)
		objectCB = make_unique<UploadBuffer<ObjectConstants>>(device, objectCount, true);
}

FrameResource::~FrameResource()
{

}

void FrameResource::Update()
{
	UpdateObjectCB();
}

void FrameResource::UpdateObjectCB()
{
	for (auto& o : RENDER->GetObjects())
	{
		if (o->numFramesDirty > 0)
		{
			XMMATRIX world = XMLoadFloat4x4(&o->GetTransform()->GetWorldMatrix());
			XMMATRIX texTransform = XMLoadFloat4x4(&o->GetTransform()->GetTexTransform());
			texTransform = XMMatrixScaling(1.0f, 1.0f, 1.0f);

			ObjectConstants objConstants;
			XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));
			XMStoreFloat4x4(&objConstants.TexTransform, XMMatrixTranspose(texTransform));

			objectCB->CopyData(o->objCBIndex, objConstants);

			o->numFramesDirty--;
		}
	}
}

void FrameResource::UpdateLightCB()
{

}
