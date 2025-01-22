#include "pch.h"
#include "Material.h"

Material::Material()
{
	matCBIndex = -1;
	diffuseSrvHeapIndex = -1;
	normalSrvHeapIndex = -1;
	numFramesDirty = GRAPHIC->GetNumFrameResources();

	diffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
	fresnel = { 0.01f, 0.01f, 0.01f };
	roughness = 0.25f;
	matTransform = MathHelper::Identity4x4();
}

Material::Material(string name, int matCBIndex, int diffuseSrvHeapIndex, int normalSrvHeapIndex) :
	name(name), matCBIndex(matCBIndex), diffuseSrvHeapIndex(diffuseSrvHeapIndex), normalSrvHeapIndex(normalSrvHeapIndex)
{
	numFramesDirty = GRAPHIC->GetNumFrameResources();

	diffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
	fresnel = { 0.01f, 0.01f, 0.01f };
	roughness = 0.25f;
	matTransform = MathHelper::Identity4x4();
}

Material::~Material()
{

}
