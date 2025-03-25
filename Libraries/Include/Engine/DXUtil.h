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

	static void UpdateBuffer(
		ComPtr<ID3D12Resource>& buffer,
		ComPtr<ID3D12Resource>& uploadBuffer,
		const void* initData, 
		UINT64 byteSize
	);

	static ComPtr<ID3DBlob> CompileShader(
		const wstring& filename,
		const D3D_SHADER_MACRO* defines,
		const string& entrypoint,
		const string& target
	);

	static void LogAdapters();

private:
	static void LogAdapterOutputs(IDXGIAdapter* adapter);
	static void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);
};
