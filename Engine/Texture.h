#pragma once
#include "Resource.h"

enum TextureType
{
	General,
	Skybox
};

class Texture : public Resource
{
	using Super = Resource;
public:
	Texture(wstring filePath, bool isName = false);
	Texture(wstring filePath, TextureType type, bool isName = false);
	virtual ~Texture();

	virtual void Load(const wstring& path) override;

	void CreateSRV();

	int GetSRVHeapIndex() { return _srvOffset; }

	static int GetCount() { return count; }
	static bool IsTextureExists(wstring& fileName);

public:
	static int count;
	TextureType textureType;
	ComPtr<ID3D12Resource> resource = nullptr;
	ComPtr<ID3D12Resource> uploadHeap = nullptr;

private:
	string _textureFormat;
	int _srvOffset;
	// int _srvHeapIndex;
};

