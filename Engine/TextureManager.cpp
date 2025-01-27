#include "pch.h"
#include "TextureManager.h"

void TextureManager::LoadTexture(string name, wstring fileDir)
{
	auto device = GRAPHIC->GetDevice().Get();
	auto commandQueue = GRAPHIC->GetCommandQueue().Get();

	auto texture = make_unique<Texture>();
	texture->Name = name;
	texture->Filename = fileDir;
	ResourceUploadBatch upload(device);
	upload.Begin();
	ThrowIfFailed(CreateDDSTextureFromFile(device, upload,
		texture->Filename.c_str(), texture->Resource.GetAddressOf()));
	_textures[texture->Name] = move(texture);
	auto finish = upload.End(commandQueue);
	finish.wait();
}

Texture* TextureManager::GetTexture(string name)
{
	return _textures[name].get();
}
