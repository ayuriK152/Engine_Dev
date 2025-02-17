#pragma once

class RenderManager
{
	DECLARE_SINGLE(RenderManager)
public:
	~RenderManager();

public:
	void Init();
	void Update();
	void Render();

public:
	ComPtr<ID3D12DescriptorHeap> GetShaderResourceViewHeap()const { return _srvHeap; }
	
	vector<shared_ptr<GameObject>>& GetObjects() { return _objects; }
	ComPtr<ID3D12PipelineState>& GetCurrPSO() { return _currPSO; }
	D3D12_GRAPHICS_PIPELINE_STATE_DESC CreatePSODesc(wstring vsName, wstring psName, wstring dsName = L"", wstring hsName = L"", wstring gsName = L"");
	void BuildPSO(string name, D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc);
	void SetCurrPSO(string name);

	shared_ptr<GameObject> AddGameObject(shared_ptr<GameObject> obj);

private:
	void BuildRootSignature();
	void BuildInputLayout();
	void BuildSRVDescriptorHeap();

	void UpdateMainCB();

	array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();

private:
	ComPtr<ID3D12RootSignature> _rootSignature;
	ComPtr<ID3D12DescriptorHeap> _srvHeap;
	vector<D3D12_INPUT_ELEMENT_DESC> _inputLayout;

	unordered_map<string, ComPtr<ID3D12PipelineState>> _PSOs;
	ComPtr<ID3D12PipelineState> _currPSO;

	PassConstants _mainPassCB;
	vector<shared_ptr<GameObject>> _objects;
};

