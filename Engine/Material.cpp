#include "pch.h"
#include "Material.h"

int Material::_count = 0;

Material::Material() : Super(ResourceType::Material)
{
	matSBIndex = _count++;

	diffuseSrvHeapIndex = 0;
	normalSrvHeapIndex = 0;
	numFramesDirty = RENDER->GetNumFrameResources();

	ambient = { 1.0f, 1.0f, 1.0f, 1.0f };
	diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	specular = { 1.0f, 1.0f, 1.0f, 1.0f };
	emissive = { 0.0f, 0.0f, 0.0f, 0.0f };
	tilling = { 1.0f, 1.0f };
	shiness = 0.25f;
	matTransform = MathHelper::Identity4x4();
}

Material::Material(string name) : Material(name, L"Tex_Default")
{

}

Material::Material(string name, wstring textureName) : Super(ResourceType::Material)
{
	matSBIndex = _count++;
	normalSrvHeapIndex = 0;
	numFramesDirty = RENDER->GetNumFrameResources();
	SetName(name);

	ambient = { 1.0f, 1.0f, 1.0f, 1.0f };
	diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	specular = { 1.0f, 1.0f, 1.0f, 1.0f };
	emissive = { 0.0f, 0.0f, 0.0f, 0.0f };
	tilling = { 1.0f, 1.0f };
	shiness = 0.25f;
	matTransform = MathHelper::Identity4x4();

	SetDiffuse(RESOURCE->Get<Texture>(textureName));
}

Material::~Material()
{

}

void Material::SetDiffuse(shared_ptr<Texture> texture)
{
	if (texture == nullptr)
	{
		SetDiffuse(L"Tex_Default");
		return;
	}

	diffuseSrvHeapIndex = texture->GetSRVHeapIndex();
	diffuseTextureName = Utils::ToString(texture->GetNameW());
}

void Material::SetDiffuse(wstring textureName)
{
	// 텍스쳐 이름만 설정하고, 실제 텍스쳐는 기본 텍스쳐로
	diffuseSrvHeapIndex = 0;
	diffuseTextureName = Utils::ToString(textureName);
}

void Material::SetNormal(shared_ptr<Texture> texture)
{
	if (texture == nullptr)
	{
		SetDiffuse(L"Tex_Default");
		return;
	}

	normalSrvHeapIndex = texture->GetSRVHeapIndex();
	normalTextureName = Utils::ToString(texture->GetNameW());
}

void Material::SetNormal(wstring textureName)
{
	normalSrvHeapIndex = 0;
	normalTextureName = Utils::ToString(textureName);
}
