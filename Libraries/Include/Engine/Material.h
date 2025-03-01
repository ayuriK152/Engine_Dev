#pragma once
#include "Resource.h"

class Texture;

struct MaterialConstants
{
	Color ambient = { 1.0f, 1.0f, 1.0f, 1.0f };
	Color diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	ColorRGB fresnel = { 0.01f, 0.01f, 0.01f };
	float roughness = 0.25f;

	XMFLOAT4X4 matTransform = MathHelper::Identity4x4();
};

class Material : public Resource
{
	using Super = Resource;
public:
	Material();
	Material(string name, int matCBIndex, int diffuseSrvHeapIndex, int normalSrvHeapIndex);
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

	Color ambient;
	Color diffuse;
	ColorRGB fresnel;
	float roughness;
	XMFLOAT4X4 matTransform;

private:
	static int _count;
};

