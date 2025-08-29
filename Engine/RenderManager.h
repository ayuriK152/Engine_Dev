#pragma once

#define		PSO_OPAQUE_SOLID		"opaque_solid"
#define		PSO_OPAQUE_SKINNED		"opaque_skinned"
#define		PSO_SKYBOX				"skybox"
#define		PSO_SHADOWMAP			"shadowmap"
#define		PSO_SHADOWMAP_SKINNED	"shadowmap_skinned"
#define		PSO_WIREFRAME			"wireframe"
#define		PSO_DEBUG_PHYSICS		"debug_physics"
#define		PSO_DEBUG_SHADOW		"debug_shadow"

#define		ROOT_PARAMETER_SKYBOX_SR		0
#define		ROOT_PARAMETER_TEXTURE_SR		1
#define		ROOT_PARAMETER_SHADOWTEX_SR		2
#define		ROOT_PARAMETER_BONE_SB			3
#define		ROOT_PARAMETER_LIGHT_CB			4
#define		ROOT_PARAMETER_LIGHTINFO_CB		5
#define		ROOT_PARAMETER_OBJECT_CB		6
#define		ROOT_PARAMETER_MATERIAL_CB		7
#define		ROOT_PARAMETER_CAMERA_CB		8

#define		STATIC_SAMPLER_COUNT			6

#define		CB_COUNT_MATERIAL				50

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
	
	const vector<shared_ptr<GameObject>>& GetObjects() { return _objects; }
	const ComPtr<ID3D12PipelineState>& GetCurrPSO() { return _currPSO; }
	const unique_ptr<UploadBuffer<MaterialConstants>>& GetMaterialCB() { return _materialCB; }

	D3D12_GRAPHICS_PIPELINE_STATE_DESC CreatePSODesc(vector<D3D12_INPUT_ELEMENT_DESC>& inputLayout, wstring vsName, wstring psName, wstring dsName = L"", wstring hsName = L"", wstring gsName = L"");
	const ComPtr<ID3D12PipelineState>& GetPSO(string name) { return _PSOs[name]; }
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

	const ShadowMap* GetShadowMap() { return _shadowMap.get(); }

	void SetPhysicsDebugRenderEnabled(bool enabled) { _isPhysicsDebugRenderEnabled = enabled; }
	bool IsPhysicsDebugRenderEnabled() { return _isPhysicsDebugRenderEnabled; }

private:
	void BuildPSO(string name, D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc);
	void BuildRootSignature();
	void BuildInputLayout();
	void BuildSRVDescriptorHeap();

	void UpdateMaterialCB();
	void UpdateCameraCB();

	array<const CD3DX12_STATIC_SAMPLER_DESC, STATIC_SAMPLER_COUNT> GetStaticSamplers();

private:
	bool _isPhysicsDebugRenderEnabled = false;

	ComPtr<ID3D12RootSignature> _rootSignature;

	ComPtr<ID3D12DescriptorHeap> _srvHeap;
	UINT _srvHeapIndex = 0;

	vector<D3D12_INPUT_ELEMENT_DESC> _solidInputLayout;
	vector<D3D12_INPUT_ELEMENT_DESC> _skinnedInputLayout;
	vector<D3D12_INPUT_ELEMENT_DESC> _skyInputLayout;
	vector<D3D12_INPUT_ELEMENT_DESC> _colliderDebugInputLayout;
	vector<D3D12_INPUT_ELEMENT_DESC> _shadowDebugInputLayout;

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

	unique_ptr<ShadowMap> _shadowMap = nullptr;
};

