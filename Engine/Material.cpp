#include "pch.h"
#include "Material.h"

int Material::_count = 0;

Material::Material() : Super(ResourceType::Material)
{
	//_count++;
	matCBIndex = _count++;

	diffuseSrvHeapIndex = -1;
	normalSrvHeapIndex = -1;
	numFramesDirty = GRAPHIC->GetNumFrameResources();

	ambient = { 1.0f, 1.0f, 1.0f, 1.0f };
	diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	specular = { 1.0f, 1.0f, 1.0f, 1.0f };
	emissive = { 0.0f, 0.0f, 0.0f, 0.0f };
	shiness = 0.25f;
	matTransform = MathHelper::Identity4x4();
}

Material::Material(string name, int diffuseSrvHeapIndex, int normalSrvHeapIndex) : Super(ResourceType::Material),
	name(name), diffuseSrvHeapIndex(diffuseSrvHeapIndex), normalSrvHeapIndex(normalSrvHeapIndex)
{
	matCBIndex = _count++;

	numFramesDirty = GRAPHIC->GetNumFrameResources();

	ambient = { 1.0f, 1.0f, 1.0f, 1.0f };
	diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	specular = { 1.0f, 1.0f, 1.0f, 1.0f };
	emissive = { 0.0f, 0.0f, 0.0f, 0.0f };
	shiness = 0.25f;
	matTransform = MathHelper::Identity4x4();
}

Material::Material(string name, wstring textureName) : Super(ResourceType::Material),
	name(name)
{
	matCBIndex = _count++;

	numFramesDirty = GRAPHIC->GetNumFrameResources();

	ambient = { 1.0f, 1.0f, 1.0f, 1.0f };
	diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	specular = { 1.0f, 1.0f, 1.0f, 1.0f };
	emissive = { 0.0f, 0.0f, 0.0f, 0.0f };
	shiness = 0.25f;
	matTransform = MathHelper::Identity4x4();

	SetTexture(RESOURCE->Get<Texture>(textureName));
}

Material::~Material()
{

}

void Material::SetTexture(shared_ptr<Texture> texture)
{
	diffuseSrvHeapIndex = texture->GetSRVHeapIndex();
}
