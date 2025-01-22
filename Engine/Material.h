#pragma once
struct MaterialConstants
{
	Color diffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
	ColorRGB fresnel = { 0.01f, 0.01f, 0.01f };
	float roughness = 0.25f;

	XMFLOAT4X4 matTransform = MathHelper::Identity4x4();
};

class Material
{
public:
	Material();
	Material(string name, int matCBIndex, int diffuseSrvHeapIndex, int normalSrvHeapIndex);
	~Material();

public:
	string name;
	int matCBIndex;
	int diffuseSrvHeapIndex;
	int normalSrvHeapIndex;
	int numFramesDirty;

	Color diffuseAlbedo;
	ColorRGB fresnel;
	float roughness;
	XMFLOAT4X4 matTransform;
};

