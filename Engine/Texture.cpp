#include "pch.h"
#include "Texture.h"

Texture::Texture(wstring fileName) : Super(ResourceType::Texture)
{
	_name = fileName;
	_path = L"Textures\\" + fileName;
	Load(_path);
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
	ThrowIfFailed(CreateDDSTextureFromFile(device, upload,
		_path.c_str(), resource.GetAddressOf()));
	auto finish = upload.End(commandQueue);
	finish.wait();
}
