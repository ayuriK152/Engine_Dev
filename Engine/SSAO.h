#pragma once

class SSAO
{
public:
	SSAO(UINT width, UINT height);

	ID3D12Resource* GetSSAOTexture() { return _ssaoMap.Get(); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetSSAOSrv() { return _ssaoSrvHandle; }

private:
	void BuildDescriptors();
	void BuildResource();
	void BuildOffsetVectors();
	void BuildRandomVectorTexture();

private:
	UINT _width, _height;

	ComPtr<ID3D12Resource> _ssaoMap;
	D3D12_CPU_DESCRIPTOR_HANDLE _ssaoRtvHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE _ssaoSrvHandle;

	ComPtr<ID3D12Resource> _randomVectorMap;
	D3D12_GPU_DESCRIPTOR_HANDLE _randomVectorSrvHandle;

	vector<Bulb::Vector4> _offsets;
};
