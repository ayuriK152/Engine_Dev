#pragma once
#include "Resource.h"

class Texture;

class BULB_API Material : public Resource
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

	void SetMetallicMap(shared_ptr<Texture> texture);
	void SetMetallicMap(wstring textureName);

	void SetRoughnessMap(shared_ptr<Texture> texture);
	void SetRoughnessMap(wstring textureName);

	void SetSpecularMap(shared_ptr<Texture> texture);
	void SetSpecularMap(wstring textureName);

	static int GetCount() { return _count; }

public:
	string diffuseTextureName;
	string normalTextureName;
	string metallicTextureName;
	string roughnessTextureName;
	string specularTextureName;

	int matSBIndex;
	int diffuseSrvHeapIndex;
	int normalSrvHeapIndex;
	int metallicSrvHeapIndex;
	int roughnessSrvHeapIndex;
	int specularSrvHeapIndex;

	int numFramesDirty;

	XMFLOAT4X4 matTransform;
	Bulb::Color ambient;
	Bulb::Color diffuse;
	Bulb::Color specular;
	Bulb::Color emissive;
	Bulb::Vector2 tilling;
	float metallic;
	float roughness;

private:
	static int _count;
};

