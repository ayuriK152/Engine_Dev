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
	void SetDiffuse(shared_ptr<Texture> texture);
	void SetDiffuse(wstring textureName);

	void SetNormal(shared_ptr<Texture> texture);
	void SetNormal(wstring textureName);

	static int GetCount() { return _count; }

public:
	string diffuseTextureName;
	string normalTextureName;

	int matSBIndex;
	int diffuseSrvHeapIndex;
	int normalSrvHeapIndex;
	int numFramesDirty;

	XMFLOAT4X4 matTransform;
	ColorRGBA ambient;
	ColorRGBA diffuse;
	ColorRGBA specular;
	ColorRGBA emissive;
	Vector2 tilling;
	float shiness;

private:
	static int _count;
};

