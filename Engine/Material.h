#pragma once
#include "Resource.h"

class Texture;

class Material : public Resource
{
	using Super = Resource;
public:
	Material();
	Material(string name, int diffuseSrvHeapIndex, int normalSrvHeapIndex);
	Material(string name, wstring textureName);
	virtual ~Material();

public:
	void SetTexture(shared_ptr<Texture> texture);

	static int GetCount() { return _count; }

public:
	string name;
	int matCBIndex;
	int diffuseSrvHeapIndex;
	int normalSrvHeapIndex;
	int numFramesDirty;

	XMFLOAT4X4 matTransform;
	Color ambient;
	Color diffuse;
	Color specular;
	Color emissive;
	float shiness;

private:
	static int _count;
};

