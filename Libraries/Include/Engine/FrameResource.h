#pragma once

struct FrameResource
{
public:
	FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount);
	FrameResource(const FrameResource& rhs) = delete;
	FrameResource& operator=(const FrameResource& rhs) = delete;
	~FrameResource();

	void Update();

private:
	void UpdateObjectCB();
	void UpdateLightCB();

public:
	ComPtr<ID3D12CommandAllocator> cmdListAlloc;

	unique_ptr<UploadBuffer<LightGatherConstants>> lightCB = nullptr;
	unique_ptr<UploadBuffer<ObjectConstants>> objectCB = nullptr;

	UINT64 fence = 0;
};