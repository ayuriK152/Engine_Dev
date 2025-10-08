#pragma once

#define DEFAULT_INSTANCE_COUNT		200
#define	DEFAULT_MATERIAL_COUNT		50
#define DEFAULT_COUNT_LIGHT			20

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
	UINT GetMaterialSRVHeapIndex()const { return _materialSrvHeapIndex; }
	UINT GetLightSRVHeapIndex()const { return _lightSrvHeapIndex; }

private:
	void UpdateObjectSB();
	void UpdateMaterialSB();
	void UpdateLightSB();

	void UpdateCameraCB();

	void BuildInstanceBufferSRV();
	void BuildMaterialBufferSRV();
	void BuildLightBufferSRV();

public:
	ComPtr<ID3D12CommandAllocator> cmdListAlloc;

	unique_ptr<UploadBuffer<InstanceConstants>> instanceSB = nullptr;
	unique_ptr<UploadBuffer<MaterialConstants>> materialSB = nullptr;
	unique_ptr<UploadBuffer<LightConstants>> lightSB = nullptr;

	unique_ptr<UploadBuffer<CameraConstants>> cameraCB = nullptr;

	UINT64 fence = 0;

private:
	UINT _instanceSrvHeapIndex = 0;
	UINT _materialSrvHeapIndex = 0;
	UINT _lightSrvHeapIndex = 0;

	bool _isInitialized = false;

	// Mesh, <IndexStart, Current Index>
	unordered_map<shared_ptr<Mesh>, pair<int, int>> _instanceIndexMap;
};