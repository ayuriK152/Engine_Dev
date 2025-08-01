#pragma once

#define DEFAULT_COUNT_OBJECT	100
#define DEFAULT_COUNT_LIGHT		20

struct FrameResource
{
public:
	FrameResource(UINT objectCount);
	FrameResource(const FrameResource& rhs) = delete;
	FrameResource& operator=(const FrameResource& rhs) = delete;
	~FrameResource();

	void Init();
	void Update();

public:
	UINT GetLightSRVHeapIndex()const { return _lightSrvHeapIndex; }

private:
	void UpdateObjectCB();
	void UpdateLightCB();

	void CreateLightSRV();

public:
	ComPtr<ID3D12CommandAllocator> cmdListAlloc;

	unique_ptr<UploadBuffer<LightConstants>> lightSB = nullptr;
	unique_ptr<UploadBuffer<ObjectConstants>> objectCB = nullptr;

	UINT64 fence = 0;

private:
	UINT _lightSrvHeapIndex = 0;

	bool _isInitialized = false;
};