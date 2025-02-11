#pragma once

class RenderManager
{
	DECLARE_SINGLE(RenderManager)
public:
	~RenderManager();

public:
	void Init();
	void Update();

private:
	void BuildRootSignature();
	void BuildInputLayout();
	void BuildDescriptorHeaps();

	void UpdateMainCB();

	array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();

private:
	ComPtr<ID3D12RootSignature> _rootSignature;
	ComPtr<ID3D12DescriptorHeap> _srvHeap;
	vector<D3D12_INPUT_ELEMENT_DESC> _inputLayout;
	unordered_map<string, ComPtr<ID3D12PipelineState>> _PSOs;
	PassConstants _mainPassCB;
};

