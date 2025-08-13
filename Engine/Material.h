#pragma once
#include "Resource.h"

class Texture;

class Material : public Resource
{
	using Super = Resource;
public:
	Material();
	Material(string name);
	Material(string name, wstring textureName);
	virtual ~Material();

public:
	void SetTexture(shared_ptr<Texture> texture);
	void SetTexture(wstring textureName);

	static int GetCount() { return _count; }

public:
	string textureName;
	int matCBIndex;
	int diffuseSrvHeapIndex;
	int normalSrvHeapIndex;
	int numFramesDirty;

	XMFLOAT4X4 matTransform;
	Color ambient;
	Color diffuse;
	Color specular;
	Color emissive;
	Vector2 tilling;
	float shiness;

private:
	static int _count;
};

