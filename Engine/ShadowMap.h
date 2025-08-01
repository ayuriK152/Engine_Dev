#pragma once

class ShadowMap
{
public:
	ShadowMap(UINT width, UINT height);

	ID3D12Resource* GetResource() { return _shadowMap.Get(); }

	D3D12_CPU_DESCRIPTOR_HANDLE GetDsv() const { return _dsvHandle; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrv() const { return _srvHandle; }

	CD3DX12_VIEWPORT GetViewport() const { return _viewport; }
	CD3DX12_RECT GetScissorRect() const { return _scissorRect; }

	void BuildDescriptors();

private:
	void BuildResource();

private:
	UINT _width = 0;
	UINT _height = 0;

	UINT _dsvHeapIndex = 0;
	UINT _srvHeapIndex = 0;
	ComPtr<ID3D12Resource> _shadowMap = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE _dsvHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE _srvHandle;

	CD3DX12_VIEWPORT _viewport;
	CD3DX12_RECT _scissorRect;
};

