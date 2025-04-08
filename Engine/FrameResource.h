#pragma once

struct FrameResource
{
public:
	FrameResource(UINT objectCount);
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