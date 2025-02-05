#pragma once
#include "Resource.h"

class Texture : public Resource
{
	using Super = Resource;
public:
	Texture(wstring fileName);
	virtual ~Texture();

	virtual void Load(const wstring& path) override;

public:
	ComPtr<ID3D12Resource> resource = nullptr;
	ComPtr<ID3D12Resource> uploadHeap = nullptr;
};

