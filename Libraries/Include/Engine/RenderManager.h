#pragma once

#define		PSO_OPAQUE_SOLID	"opaque_solid"
#define		PSO_OPAQUE_SKINNED	"opaque_skinned"
#define		PSO_SKYBOX			"skybox"
#define		PSO_WIREFRAME		"wireframe"

#define		ROOT_PARAMETER_SKYBOX_SR	0
#define		ROOT_PARAMETER_TEXTURE_SR	1
#define		ROOT_PARAMETER_BONE_SB		2
#define		ROOT_PARAMETER_LIGHT_CB		3
#define		ROOT_PARAMETER_OBJECT_CB	4
#define		ROOT_PARAMETER_MATERIAL_CB	5
#define		ROOT_PARAMETER_CAMERA_CB	6

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
	void UpdateObjectPSO(shared_ptr<GameObject> obj, string targetPSO);

	UINT GetAndIncreaseSRVHeapIndex() { return _srvHeapIndex++; }

	shared_ptr<GameObject> AddGameObject(shared_ptr<GameObject> obj);

	vector<Light*>& GetLights() { return _lights; }
	void AddLight(Light* light) { _lights.push_back(light); }

	int GetSkyboxTexSRVHeapIndex() { return _skyboxTexSrvHeapIndex; }
	void SetSkyboxTexture(shared_ptr<Texture> tex) {
		_skyboxTexSrvHeapIndex = tex->GetSRVHeapIndex();
	}

private:
	void BuildPSO(string name, D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc);
	void BuildRootSignature();
	void BuildInputLayout();
	void BuildSRVDescriptorHeap();

	void UpdateMaterialCB();
	void UpdateCameraCB();

	array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();

private:
	ComPtr<ID3D12RootSignature> _rootSignature;

	ComPtr<ID3D12DescriptorHeap> _srvHeap;
	UINT _srvHeapIndex = 0;

	vector<D3D12_INPUT_ELEMENT_DESC> _solidInputLayout;
	vector<D3D12_INPUT_ELEMENT_DESC> _skinnedInputLayout;
	vector<D3D12_INPUT_ELEMENT_DESC> _skyInputLayout;

	bool _isPSOFixed = false;
	unordered_map<string, ComPtr<ID3D12PipelineState>> _PSOs;
	ComPtr<ID3D12PipelineState> _currPSO;

	vector<shared_ptr<GameObject>> _objects;
	unordered_map<string, vector<shared_ptr<GameObject>>> _sortedObjects;
	vector<Light*> _lights;

	int _skyboxTexSrvHeapIndex = -1;

	// Constant Buffers
	unique_ptr<UploadBuffer<MaterialConstants>> _materialCB = nullptr;

	CameraConstants _cameraConstants;
	unique_ptr<UploadBuffer<CameraConstants>> _cameraCB = nullptr;
};

