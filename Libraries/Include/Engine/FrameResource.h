#pragma once

#define DEFAULT_INSTANCE_COUNT	200
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
	UINT GetInstanceSRVHeapIndex()const { return _instanceSrvHeapIndex; }
	UINT GetLightSRVHeapIndex()const { return _lightSrvHeapIndex; }

private:
	void UpdateObjectCB();
	void UpdateLightCB();

	void BuildInstanceBufferSRV();
	void BuildLightBufferSRV();

public:
	ComPtr<ID3D12CommandAllocator> cmdListAlloc;

	unique_ptr<UploadBuffer<LightConstants>> lightSB = nullptr;
	unique_ptr<UploadBuffer<InstanceConstants>> instanceSB = nullptr;

	UINT64 fence = 0;

private:
	UINT _lightSrvHeapIndex = 0;
	UINT _instanceSrvHeapIndex = 0;

	bool _isInitialized = false;

	// Mesh, <IndexStart, Current Index>
	unordered_map<shared_ptr<Mesh>, pair<int, int>> _instanceIndexMap;
};