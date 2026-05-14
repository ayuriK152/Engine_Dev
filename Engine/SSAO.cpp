#include "pch.h"
#include "SSAO.h"

SSAO::SSAO(UINT width, UINT height)
{
	_width = width;
	_height = height;

	BuildResource();
}

void SSAO::BuildDescriptors()
{
	auto device = GRAPHIC->GetDevice();
	device->CreateRenderTargetView(_ssaoMap.Get(), nullptr, _ssaoRtvHandle);

	//_srvHeapIndex = RENDER->GetAndIncreaseSRVHeapIndex();
	//CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuSrv(RENDER->GetCommonSRVHeap()->GetCPUDescriptorHandleForHeapStart());
	//hCpuSrv.Offset(_srvHeapIndex, GRAPHIC->GetCBVSRVDescriptorSize());

	//D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	//srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	//srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	//srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	//srvDesc.Texture2D.MipLevels = 1;

	//device->CreateShaderResourceView(_ssaoMap.Get());
}

void SSAO::BuildResource()
{
	auto device = GRAPHIC->GetDevice();

	D3D12_RESOURCE_DESC texDesc =
		CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8_UNORM, _width, _height, 1, 1);
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	D3D12_CLEAR_VALUE clearVal = { DXGI_FORMAT_R8_UNORM, {1.0f, 1.0f, 1.0f, 1.0f} };

	ThrowIfFailed(GRAPHIC->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		&clearVal,
		IID_PPV_ARGS(&_ssaoMap)));
}

void SSAO::BuildOffsetVectors()
{

}

void SSAO::BuildRandomVectorTexture()
{

}
