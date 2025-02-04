#pragma once
#include "Resource.h"

class Texture : public Resource
{
	using Super = Resource;
public:
	Texture();
	virtual ~Texture();

public:
	ComPtr<ID3D12Resource> Resource = nullptr;
	ComPtr<ID3D12Resource> UploadHeap = nullptr;
};

