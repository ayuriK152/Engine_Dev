#pragma once
class DXUtil
{
public:
	static UINT CalcConstantBufferByteSize(UINT byteSize)
	{
		return (byteSize + 255) & ~255;
	}

	static ComPtr<ID3D12Resource> CreateDefaultBuffer(
		ID3D12Device* device,
		ID3D12GraphicsCommandList* cmdList,
		const void* initData,
		UINT64 byteSize,
		ComPtr<ID3D12Resource>& uploadBuffer
	);

	static ComPtr<ID3DBlob> CompileShader(
		const wstring& filename,
		const D3D_SHADER_MACRO* defines,
		const string& entrypoint,
		const string& target
	);

	static void BuildPSO(
		string name,
		vector<D3D12_INPUT_ELEMENT_DESC> inputLayout,
		ComPtr<ID3D12RootSignature> rootSignature,
		ComPtr<ID3DBlob> vertexShader,
		ComPtr<ID3DBlob> pixelShader,
		DXGI_FORMAT backBufferFormat,
		DXGI_FORMAT depthStencilFormat,
		unordered_map<string, ComPtr<ID3D12PipelineState>>& PSOs
	);
};
