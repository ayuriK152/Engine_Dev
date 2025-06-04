#include "pch.h"
#include "FrameResource.h"

FrameResource::FrameResource(UINT objectCount)
{
	ThrowIfFailed(GRAPHIC->GetDevice()->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(cmdListAlloc.GetAddressOf())));

	lightCB = make_unique<UploadBuffer<LightGatherConstants>>(1, true);
	if (objectCount > 0)
		objectCB = make_unique<UploadBuffer<ObjectConstants>>(objectCount, true);
}

FrameResource::~FrameResource()
{

}

void FrameResource::Update()
{
	UpdateObjectCB();
	UpdateLightCB();
}

void FrameResource::UpdateObjectCB()
{
	for (auto& o : RENDER->GetObjects())
	{
		if (o->numFramesDirty > 0)
		{
			XMMATRIX world = XMLoadFloat4x4(&o->GetTransform()->GetWorldMatrix());

			ObjectConstants objConstants;
			XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));

			objectCB->CopyData(o->objCBIndex, objConstants);

			o->numFramesDirty--;
		}
	}
}

void FrameResource::UpdateLightCB()
{
	bool flag = false;
	LightGatherConstants lightConstants;
	auto lights = RENDER->GetLights();

	if (lights.size() == 0)
		return;

	// Global Light
	if (lights[0]->numFramesDirty > 0)
	{
		lightConstants.GlobalLight = lights[0]->GetLightConstants();
		lights[0]->numFramesDirty--;
		flag = true;
	}

	// General Light
	for (int i = 1; i < lights.size(); i++)
	{
		if (lights[i]->numFramesDirty > 0)
		{
			lightConstants.Lights[i - 1] = lights[i]->GetLightConstants();
			lights[i]->numFramesDirty--;
			flag = true;
		}
	}

	if (flag)
	{
		lightCB->CopyData(0, lightConstants);
	}
}
