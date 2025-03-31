#pragma once

struct PassConstants
{
	XMFLOAT4 AmbientLight = { 0.0f, 0.0f, 0.0f, 1.0f };
	LightConstants Lights[1];
};

struct FrameResource
{
public:
	FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount);
	FrameResource(const FrameResource& rhs) = delete;
	FrameResource& operator=(const FrameResource& rhs) = delete;
	~FrameResource();

	void Update();

public:
	ComPtr<ID3D12CommandAllocator> cmdListAlloc;

	unique_ptr<UploadBuffer<PassConstants>> passCB = nullptr;
	unique_ptr<UploadBuffer<ObjectConstants>> objectCB = nullptr;

	UINT64 fence = 0;
};