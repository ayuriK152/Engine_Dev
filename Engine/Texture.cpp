#include "pch.h"
#include "Texture.h"

int Texture::count = 0;

bool Texture::IsTextureExists(wstring& fileName)
{
	if (filesystem::exists(L"..\\Resources\\Textures\\" + fileName))
		return true;
	else
		return false;
}

Texture::Texture(wstring fileName) : Super(ResourceType::Texture)
{
	SetName(fileName);
	_textureFormat = UniversalUtils::ToString(fileName.substr(fileName.find_last_of('.') + 1));
	SetPath(L"..\\Resources\\Textures\\" + fileName);
	Load(_pathw);
	textureType = TextureType::General;
	CreateSRV();
}

Texture::Texture(wstring fileName, TextureType type) : Super(ResourceType::Texture)
{
	SetName(fileName);
	_textureFormat = UniversalUtils::ToString(fileName.substr(fileName.find_last_of('.') + 1));
	SetPath(L"..\\Resources\\Textures\\" + fileName);
	Load(_pathw);
	textureType = type;
	CreateSRV();
}

Texture::~Texture()
{

}

void Texture::Load(const wstring& path)
{
	auto device = GRAPHIC->GetDevice().Get();
	auto commandQueue = GRAPHIC->GetCommandQueue().Get();

	ResourceUploadBatch upload(device);
	upload.Begin();

	if (_textureFormat == "dds")
	{
		ThrowIfFailed(CreateDDSTextureFromFile(device, upload,
			_pathw.c_str(), resource.GetAddressOf()));
	}
	else
	{
		ThrowIfFailed(CreateWICTextureFromFile(device, upload,
			_pathw.c_str(), resource.GetAddressOf()));
	}
	auto finish = upload.End(commandQueue);
	finish.wait();
}

void Texture::CreateSRV()
{
	_srvOffset = RENDER->GetAndIncreaseSRVHeapIndex();
	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(RENDER->GetCommonSRVHeap()->GetCPUDescriptorHandleForHeapStart());
	hDescriptor.Offset(_srvOffset, GRAPHIC->GetCBVSRVDescriptorSize());

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = resource->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = resource->GetDesc().MipLevels;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

	if (textureType == TextureType::Skybox)
	{
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
	}

	GRAPHIC->GetDevice()->CreateShaderResourceView(resource.Get(), &srvDesc, hDescriptor);
}
