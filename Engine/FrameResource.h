#pragma once

#define DIRECT_LIGHT 0
#define POINT_LIGHT 1
#define SPOT_LIGHT 2

struct ObjectConstants
{
	XMFLOAT4X4 World = MathHelper::Identity4x4();
	XMFLOAT4X4 TexTransform = MathHelper::Identity4x4();
};

struct Light
{
	XMFLOAT4 Ambient = { 0.0f, 0.0f, 0.0f, 1.0f };
	XMFLOAT4 Diffuse = { 0.0f, 0.0f, 0.0f, 1.0f };
	XMFLOAT4 Specular = { 0.0f, 0.0f, 0.0f, 1.0f };
	XMFLOAT4 Emissive = { 0.0f, 0.0f, 0.0f, 1.0f };
	XMFLOAT3 Direction = { 0.0f, 0.0f, 0.0f };
	UINT LightType = DIRECT_LIGHT;
	XMFLOAT2 FalloffInfo = { 0.0f, 0.0f };
	float SpotPower = 0.0f;
	int padding = 0;
};

struct PassConstants
{
	//XMFLOAT4X4 View = MathHelper::Identity4x4();
	//XMFLOAT4X4 InvView = MathHelper::Identity4x4();
	//XMFLOAT4X4 Proj = MathHelper::Identity4x4();
	//XMFLOAT4X4 InvProj = MathHelper::Identity4x4();
	//XMFLOAT4X4 ViewProj = MathHelper::Identity4x4();
	//XMFLOAT4X4 InvViewProj = MathHelper::Identity4x4();

	XMFLOAT4 AmbientLight = { 0.0f, 0.0f, 0.0f, 1.0f };
	Light Lights[1];
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