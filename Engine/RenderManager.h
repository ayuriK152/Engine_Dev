#pragma once

#define		PSO_OPAQUE_SOLID	"opaque_solid"
#define		PSO_OPAQUE_SKINNED	"opaque_skinned"
#define		PSO_WIREFRAME		"wireframe"

struct CameraConstants
{
	XMFLOAT4X4 View = MathHelper::Identity4x4();
	XMFLOAT4X4 InvView = MathHelper::Identity4x4();
	XMFLOAT4X4 Proj = MathHelper::Identity4x4();
	XMFLOAT4X4 InvProj = MathHelper::Identity4x4();
	XMFLOAT4X4 ViewProj = MathHelper::Identity4x4();
	XMFLOAT4X4 InvViewProj = MathHelper::Identity4x4();
	XMFLOAT2 RenderTargetSize = { 0.0f, 0.0f };
	XMFLOAT2 InvRenderTargetSize = { 0.0f, 0.0f };
};

class RenderManager
{
	DECLARE_SINGLE(RenderManager)
public:
	~RenderManager() = default;

public:
	void Init();
	void FixedUpdate();
	void Update();
	void Render();

public:
	ComPtr<ID3D12DescriptorHeap> GetShaderResourceViewHeap()const { return _srvHeap; }
	
	vector<shared_ptr<GameObject>>& GetObjects() { return _objects; }
	ComPtr<ID3D12PipelineState>& GetCurrPSO() { return _currPSO; }
	unique_ptr<UploadBuffer<MaterialConstants>>& GetMaterialCB() { return _materialCB; }

	D3D12_GRAPHICS_PIPELINE_STATE_DESC CreatePSODesc(vector<D3D12_INPUT_ELEMENT_DESC>& inputLayout, wstring vsName, wstring psName, wstring dsName = L"", wstring hsName = L"", wstring gsName = L"");
	void SetCurrPSO(string name);
	void SetDefaultPSO();

	UINT GetAndIncreaseSRVHeapIndex() { return _srvHeapIndex++; }

	shared_ptr<GameObject> AddGameObject(shared_ptr<GameObject> obj);

private:
	void BuildPSO(string name, D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc);
	void BuildRootSignature();
	void BuildInputLayout();
	void BuildSRVDescriptorHeap();
	void BuildPrimitiveBatch();

	void UpdateMainCB();
	void UpdateMaterialCB();
	void UpdateCameraCB();

	array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();

private:
	ComPtr<ID3D12RootSignature> _rootSignature;

	ComPtr<ID3D12DescriptorHeap> _srvHeap;
	UINT _srvHeapIndex = 0;

	vector<D3D12_INPUT_ELEMENT_DESC> _inputLayout;
	vector<D3D12_INPUT_ELEMENT_DESC> _skinnedInputLayout;

	unique_ptr<PrimitiveBatch<DirectX::VertexPositionColor>> _primitiveBatch;
	unique_ptr<BasicEffect> _lineEffect;

	bool _isPSOFixed = false;
	unordered_map<string, ComPtr<ID3D12PipelineState>> _PSOs;
	ComPtr<ID3D12PipelineState> _currPSO;

	// Constant Buffers
	PassConstants _mainPassCB;
	unique_ptr<UploadBuffer<MaterialConstants>> _materialCB = nullptr;
	vector<shared_ptr<GameObject>> _objects;

	CameraConstants _cameraCB;
	unique_ptr<UploadBuffer<CameraConstants>> _cameraCBUploadBuffer = nullptr;
};

