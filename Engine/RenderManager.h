#pragma once

#define		PSO_OPAQUE_SOLID		"opaque_solid"
#define		PSO_OPAQUE_SKINNED		"opaque_skinned"
#define		PSO_SKYBOX				"skybox"
#define		PSO_SHADOWMAP			"shadowmap"
#define		PSO_SHADOWMAP_SKINNED	"shadowmap_skinned"
#define		PSO_WIREFRAME			"wireframe"
#define		PSO_DEBUG_PHYSICS		"debug_physics"
#define		PSO_DEBUG_SHADOW		"debug_shadow"

#pragma region Root_Parameters
#define		ROOT_PARAMETER_COUNT			12

#define		ROOT_PARAM_INSTCANCE_SB		0
#define		ROOT_PARAM_MATERIAL_SB		1
#define		ROOT_PARAM_LIGHT_SB			2
#define		ROOT_PARAM_SKYBOX_SR		3
#define		ROOT_PARAM_SHADOWMAP_SR		4
#define		ROOT_PARAM_TEXTURE_ARR		5

#define		ROOT_PARAM_LIGHTINFO_C		6
#define		ROOT_PARAM_CAMERA_CB		7
#define		ROOT_PARAM_MESHINFO_C		8

#define		ROOT_PARAM_BONE_SB			9
#define		ROOT_PARAM_ANIM_SB			10
#define		ROOT_PARAM_ANIMSTATE_CB		11
#pragma endregion

#pragma region Register_Numbers
#define		REGISTER_NUM_INSTANCE_SB	0
#define		REGISTER_NUM_MAT_SB			1
#define		REGISTER_NUM_LIGHT_SB		2
#define		REGISTER_NUM_SKYBOX_SR		3
#define		REGISTER_NUM_SHADOWMAP_SR	4
#define		REGISTER_NUM_TEXTURE_ARR	5

#define		REGISTER_NUM_LIGHTINFO_CB	0
#define		REGISTER_NUM_CAMERA_CB		1
#define		REGISTER_NUM_MESHINFO_CB	2

#define		REGISTER_NUM_BONE_SB		0
#define		REGISTER_NUM_ANIM_SB		1

#define		REGISTER_NUM_ANIMSTATE_CB	0
#pragma endregion

#define		DESCRIPTOR_HEAP_SIZE			512
#define		DEFAULT_MATERIAL_COUNT			50
#define		DEFAULT_TEXTURE_ARR_SIZE		100
#define		STATIC_SAMPLER_COUNT			6

#define		DEFAULT_ANIMATION_COUNT			500		// Not used yet

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
	ComPtr<ID3D12DescriptorHeap> GetCommonSRVHeap()const { return _srvHeap; }
	
	const vector<shared_ptr<GameObject>>& GetObjects() { return _objects; }
	const ComPtr<ID3D12PipelineState>& GetCurrPSO() { return _currPSO; }
	const unique_ptr<UploadBuffer<MaterialConstants>>& GetMaterialCB() { return _materialSB; }

	D3D12_GRAPHICS_PIPELINE_STATE_DESC CreatePSODesc(vector<D3D12_INPUT_ELEMENT_DESC>& inputLayout, wstring vsName = L"", wstring psName = L"", wstring dsName = L"", wstring hsName = L"", wstring gsName = L"");
	D3D12_COMPUTE_PIPELINE_STATE_DESC CreateCSPSODesc(wstring csName);
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

	void UpdateMeshInstanceStartIndices();
	UINT GetMeshInstanceStartIndex(const shared_ptr<Mesh>& mesh) {
		if (_meshInstanceStartIndex.contains(mesh))
			return _meshInstanceStartIndex[mesh];
		return 0;
	}

	void RefreshMeshRenderCheckMap();
	bool IsMeshRendered(const shared_ptr<Mesh>& mesh) { 
		if (!_meshRenderCheckMap.contains(mesh))
			_meshRenderCheckMap[mesh] = false;
		return _meshRenderCheckMap[mesh]; 
	}
	void SetMeshRenderCheckValue(const shared_ptr<Mesh>& mesh) { _meshRenderCheckMap[mesh] = true; }

	void SetPhysicsDebugRenderEnabled(bool enabled) { _isPhysicsDebugRenderEnabled = enabled; }
	bool IsPhysicsDebugRenderEnabled() { return _isPhysicsDebugRenderEnabled; }

	// Return Animation SB index
	UINT UploadAnimationData(const map<int, map<int, XMMATRIX>>& animationMap);
	void SetAnimationState(const AnimationStateConstants& state);

private:
	void BuildPSO(string name, D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc);
	void BuildPSO(string name, D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc);
	void BuildRootSignature();
	void BuildInputLayout();
	void BuildSRVDescriptorHeap();

	void BuildMaterialBufferSRV();
	void BuildAnimationBufferSRV();

	void UpdateMaterialCB();
	void UpdateCameraCB();

	array<const CD3DX12_STATIC_SAMPLER_DESC, STATIC_SAMPLER_COUNT> GetStaticSamplers();

private:
	bool _isPhysicsDebugRenderEnabled = false;

	ComPtr<ID3D12RootSignature> _rootSignature;

	// Input Layout
	vector<D3D12_INPUT_ELEMENT_DESC> _solidInputLayout;
	vector<D3D12_INPUT_ELEMENT_DESC> _skinnedInputLayout;
	vector<D3D12_INPUT_ELEMENT_DESC> _skyInputLayout;
	vector<D3D12_INPUT_ELEMENT_DESC> _colliderDebugInputLayout;

	bool _isPSOFixed = false;
	unordered_map<string, ComPtr<ID3D12PipelineState>> _PSOs;
	ComPtr<ID3D12PipelineState> _currPSO;

	vector<shared_ptr<GameObject>> _objects;
	unordered_map<string, vector<shared_ptr<GameObject>>> _sortedObjects;
	unordered_map<shared_ptr<Mesh>, UINT> _meshInstanceStartIndex;
	unordered_map<shared_ptr<Mesh>, bool> _meshRenderCheckMap;

	vector<Light*> _lights;

	// SRV Heap
	ComPtr<ID3D12DescriptorHeap> _srvHeap;
	UINT _srvHeapIndex = 0;

	int _skyboxTexSrvHeapIndex = -1;
	unique_ptr<ShadowMap> _shadowMap = nullptr;

	UINT _instanceSrvHeapIndex = 0;
	unique_ptr<UploadBuffer<InstanceConstants>> _instanceSB = nullptr;

	UINT _materialSrvHeapIndex = 0;
	unique_ptr<UploadBuffer<MaterialConstants>> _materialSB = nullptr;

	UINT _animationBufferOffset = 0;
	UINT _animationSrvHeapIndex = 0;
	unique_ptr<UploadBuffer<XMFLOAT4X4>> _animationSB = nullptr;

	// Constant Buffers
	CameraConstants _cameraConstants;
	unique_ptr<UploadBuffer<CameraConstants>> _cameraCB = nullptr;

	unique_ptr<UploadBuffer<AnimationStateConstants>> _animationStateCB = nullptr;

	ComPtr<ID3D12Resource> _animationTransformBuffer = nullptr;
};

