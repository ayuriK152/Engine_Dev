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
	void UpdateMainCB();

private:
	ComPtr<ID3D12RootSignature> _rootSignature;
	vector<D3D12_INPUT_ELEMENT_DESC> _inputLayout;
	unordered_map<string, ComPtr<ID3D12PipelineState>> _PSOs;
};

