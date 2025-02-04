#include "pch.h"
#include "TextureManager.h"

void TextureManager::LoadTexture(wstring name, wstring fileDir)
{
	auto device = GRAPHIC->GetDevice().Get();
	auto commandQueue = GRAPHIC->GetCommandQueue().Get();

	auto texture = make_shared<Texture>();
	texture->SetName(name);
	texture->SetPath(fileDir);
	ResourceUploadBatch upload(device);
	upload.Begin();
	ThrowIfFailed(CreateDDSTextureFromFile(device, upload,
		texture->GetPath().c_str(), texture->Resource.GetAddressOf()));
	_textures[texture->GetName()] = move(texture);
	auto finish = upload.End(commandQueue);
	finish.wait();
}

shared_ptr<Texture> TextureManager::GetTexture(wstring name)
{
	return _textures[name];
}
