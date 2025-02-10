#pragma once
#include "Resource.h"

class Texture : public Resource
{
	using Super = Resource;
public:
	Texture(wstring fileName);
	virtual ~Texture();

	virtual void Load(const wstring& path) override;

	void CreateSRV();

	int GetSRVHeapIndex() { return _srvHeapIndex; }

	static int GetCount() { return _count; }

public:
	static int _count;
	ComPtr<ID3D12Resource> resource = nullptr;
	ComPtr<ID3D12Resource> uploadHeap = nullptr;

private:

	int _srvHeapIndex;
};

