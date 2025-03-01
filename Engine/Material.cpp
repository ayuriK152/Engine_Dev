#include "pch.h"
#include "Material.h"

int Material::_count = 0;

Material::Material() : Super(ResourceType::Material)
{
	_count++;

	matCBIndex = -1;
	diffuseSrvHeapIndex = -1;
	normalSrvHeapIndex = -1;
	numFramesDirty = GRAPHIC->GetNumFrameResources();

	ambient = { 1.0f, 1.0f, 1.0f, 1.0f };
	diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	fresnel = { 0.01f, 0.01f, 0.01f };
	roughness = 0.25f;
	matTransform = MathHelper::Identity4x4();
}

Material::Material(string name, int matCBIndex, int diffuseSrvHeapIndex, int normalSrvHeapIndex) : Super(ResourceType::Material),
	name(name), matCBIndex(matCBIndex), diffuseSrvHeapIndex(diffuseSrvHeapIndex), normalSrvHeapIndex(normalSrvHeapIndex)
{
	numFramesDirty = GRAPHIC->GetNumFrameResources();

	diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	fresnel = { 0.01f, 0.01f, 0.01f };
	roughness = 0.25f;
	matTransform = MathHelper::Identity4x4();
}

Material::~Material()
{

}

void Material::SetTexture(shared_ptr<Texture> texture)
{
	diffuseSrvHeapIndex = texture->GetSRVHeapIndex();
}
