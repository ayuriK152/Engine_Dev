#pragma once

#define		PSO_COUNT				10

#define		PSO_OPAQUE_SOLID		"opaque_solid"
#define		PSO_OPAQUE_SKINNED		"opaque_skinned"
#define		PSO_SKYBOX				"skybox"
#define		PSO_SHADOWMAP			"shadowmap"
#define		PSO_SHADOWMAP_SKINNED	"shadowmap_skinned"
#define		PSO_WIREFRAME			"wireframe"
#define		PSO_DEBUG_PHYSICS		"debug_physics"
#define		PSO_DEBUG_SHADOW		"debug_shadow"
#define		PSO_PARTICLE_UPDATE		"particle_update"
#define		PSO_PARTICLE_RENDER		"particle_render"
#define		PSO_UI					"ui"

#define		PSO_IDX_OPAQUE_SOLID		0
#define		PSO_IDX_OPAQUE_SKINNED		1
#define		PSO_IDX_SKYBOX				2
#define		PSO_IDX_SHADOWMAP			3
#define		PSO_IDX_SHADOWMAP_SKINNED	4
#define		PSO_IDX_WIREFRAME			5
#define		PSO_IDX_DEBUG_PHYSICS		6
#define		PSO_IDX_DEBUG_SHADOW		7
#define		PSO_IDX_PARTICLE_UPDATE		8
#define		PSO_IDX_PARTICLE_RENDER		9
#define		PSO_IDX_UI					10

#pragma region Root_Parameters
#define		ROOT_PARAMETER_COUNT		13

#define		ROOT_PARAM_INSTCANCE_SB		0
#define		ROOT_PARAM_MATERIAL_SB		1
#define		ROOT_PARAM_LIGHT_SB			2
#define		ROOT_PARAM_SKYBOX_SR		3
#define		ROOT_PARAM_SHADOWMAP_SR		4
#define		ROOT_PARAM_TEXTURE_ARR		5

#define		ROOT_PARAM_CLIENTINFO_C		6
#define		ROOT_PARAM_LIGHTINFO_C		7
#define		ROOT_PARAM_CAMERA_CB		8
#define		ROOT_PARAM_MESHINFO_C		9

#define		ROOT_PARAM_BONE_SB			10

#define		ROOT_PARAM_PARTICLES_RW		11
#define		ROOT_PARAM_EMITTER_CB		12
#pragma endregion

#pragma region Register_Numbers
#define		REGISTER_NUM_INSTANCE_SB	0
#define		REGISTER_NUM_MAT_SB			1
#define		REGISTER_NUM_LIGHT_SB		2
#define		REGISTER_NUM_SKYBOX_SR		3
#define		REGISTER_NUM_SHADOWMAP_SR	4
#define		REGISTER_NUM_TEXTURE_ARR	5

#define		REGISTER_NUM_CLIENTINFO_C	0
#define		REGISTER_NUM_LIGHTINFO_C	1
#define		REGISTER_NUM_CAMERA_CB		2
#define		REGISTER_NUM_MESHINFO_C		3

#define		REGISTER_NUM_BONE_SB		0

#define		REGISTER_NUM_PARTICLES_RW	0
#define		REGISTER_NUM_EMITTER_CB		0
#pragma endregion

#define		DESCRIPTOR_HEAP_SIZE			512
#define		DEFAULT_TEXTURE_ARR_SIZE		100
#define		STATIC_SAMPLER_COUNT			6

#define		DEFAULT_ANIMATION_COUNT			500		// Not used yet

#define		MAX_HIERARCHY_DEPTH		32

class RenderManager
{
	DECLARE_SINGLE(RenderManager)
public:
	~RenderManager() = default;

public:
	void Init();
	void PreUpdate();
	void Update();
	void Render();

public:
	ComPtr<ID3D12DescriptorHeap> GetCommonSRVHeap()const { return _srvHeap; }
	
	const vector<shared_ptr<GameObject>>& GetObjects() { return _objects; }
	const ComPtr<ID3D12PipelineState>& GetCurrPSO() { return _currPSO; }
	const ComPtr<ID3D12RootSignature>& GetRootSignature() { return _rootSignature; }

	// Create PSO Descriptor
	D3D12_GRAPHICS_PIPELINE_STATE_DESC CreatePSODesc(vector<D3D12_INPUT_ELEMENT_DESC>& inputLayout, wstring vsName = L"", wstring psName = L"", wstring dsName = L"", wstring hsName = L"", wstring gsName = L"");
	// Create PSO Descriptor without InputLayout
	D3D12_GRAPHICS_PIPELINE_STATE_DESC CreatePSODesc(wstring vsName = L"", wstring psName = L"", wstring dsName = L"", wstring hsName = L"", wstring gsName = L"");
	// Create PSO Descriptor for Compute Shader
	D3D12_COMPUTE_PIPELINE_STATE_DESC CreateCSPSODesc(wstring csName);
	// GetPSO with PSO name string. Refactoring on plan
	const ComPtr<ID3D12PipelineState>& GetPSO(string name) { return _PSOs[name]; }
	void SetCurrPSO(string name);
	void SetDefaultPSO();
	void UpdateObjectPSO(shared_ptr<GameObject> obj, string targetPSO);
	UINT Temp_GetPSOIndex(string name);

	void BuildFrameResources();
	int GetNumFrameResources() { return _numFrameResources; }

	UINT GetAndIncreaseSRVHeapIndex() { return _srvHeapIndex++; }

	shared_ptr<GameObject> AddGameObject(shared_ptr<GameObject> obj);
	void DeleteGameobject(shared_ptr<GameObject> obj);

	vector<Light*>& GetLights() { return _lights; }
	void AddLight(Light* light) { _lights.push_back(light); }

	int GetSkyboxTexSRVHeapIndex() { return _skyboxTexSrvHeapIndex; }
	void SetSkyboxTexture(shared_ptr<Texture> tex) {
		_skyboxTexSrvHeapIndex = tex->GetSRVHeapIndex();
	}

	const ShadowMap* GetShadowMap() { return _shadowMap.get(); }

	void AddMeshInfo(int id) {
		while (_meshInstanceStartIndex.size() <= id)
			_meshInstanceStartIndex.push_back(0);
		while (_meshRenderCheckMap.size() <= id)
			_meshRenderCheckMap.push_back(false);
	}

	void UpdateMeshInstanceStartIndices();
	UINT GetMeshInstanceStartIndex(const shared_ptr<Mesh>& mesh) { return _meshInstanceStartIndex[mesh->GetID()]; }

	void RefreshMeshRenderCheckMap();

	// Returns whether mesh rendered or not
	bool CheckMeshRender(const shared_ptr<Mesh>& mesh) { return _meshRenderCheckMap[mesh->GetID()] == 1; }

	void SetMeshRenderCheckValue(const shared_ptr<Mesh>& mesh) { _meshRenderCheckMap[mesh->GetID()]++; }

	void SetPhysicsDebugRenderEnabled(bool enabled) { _isPhysicsDebugRenderEnabled = enabled; }
	bool IsPhysicsDebugRenderEnabled() { return _isPhysicsDebugRenderEnabled; }

private:
	void BuildPSO(string name, D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc);
	void BuildPSO(string name, D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc);

	void BuildRootSignature();
	void BuildInputLayout();
	void BuildSRVDescriptorHeap();
	void BuildPSOs();

	void SetCommonState(ID3D12GraphicsCommandList* cmdList);

	array<const CD3DX12_STATIC_SAMPLER_DESC, STATIC_SAMPLER_COUNT> GetStaticSamplers();

private:
	bool _isPhysicsDebugRenderEnabled = false;

	ID3D12CommandAllocator* _mainCmdListAlloc;
	ID3D12GraphicsCommandList* _mainCmdList;

	ComPtr<ID3D12RootSignature> _rootSignature;

	// Input Layout
	vector<D3D12_INPUT_ELEMENT_DESC> _solidInputLayout;
	vector<D3D12_INPUT_ELEMENT_DESC> _skinnedInputLayout;
	vector<D3D12_INPUT_ELEMENT_DESC> _skyInputLayout;
	vector<D3D12_INPUT_ELEMENT_DESC> _colliderDebugInputLayout;
	vector<D3D12_INPUT_ELEMENT_DESC> _uiInputLayout;

	bool _isPSOFixed = false;
	// Refactoring on plan
	unordered_map<string, ComPtr<ID3D12PipelineState>> _PSOs;
	ComPtr<ID3D12PipelineState> _currPSO;

	static const int _numFrameResources = 3;
	int _currFrameResourceIndex = 0;
	FrameResource* _currFrameResource = nullptr;
	vector<unique_ptr<FrameResource>> _frameResources;

	vector<shared_ptr<GameObject>> _objects;
	array<vector<shared_ptr<GameObject>>, PSO_COUNT> _objectsSortedPSO;
	vector<UINT> _meshInstanceStartIndex;
	vector<UINT> _meshRenderCheckMap;

	vector<Light*> _lights;

	// SRV Heap
	ComPtr<ID3D12DescriptorHeap> _srvHeap;
	UINT _srvHeapIndex = 0;

	int _skyboxTexSrvHeapIndex = -1;
	unique_ptr<ShadowMap> _shadowMap = nullptr;

	ComPtr<ID3D12Resource> _animationTransformBuffer = nullptr;

	array<future<void>, 4> _futures;
};

