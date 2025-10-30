#include "pch.h"
#include "Material.h"

int Material::_count = 0;

Material::Material() : Super(ResourceType::Material)
{
	matSBIndex = _count++;

	diffuseSrvHeapIndex = 0;
	normalSrvHeapIndex = 0;
	numFramesDirty = GRAPHIC->GetNumFrameResources();

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
	numFramesDirty = GRAPHIC->GetNumFrameResources();
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
	diffuseTextureName = UniversalUtils::ToString(texture->GetNameW());
}

void Material::SetDiffuse(wstring textureName)
{
	// �ؽ��� �̸��� �����ϰ�, ���� �ؽ��Ĵ� �⺻ �ؽ��ķ�
	diffuseSrvHeapIndex = 0;
	diffuseTextureName = UniversalUtils::ToString(textureName);
}

void Material::SetNormal(shared_ptr<Texture> texture)
{
	if (texture == nullptr)
	{
		SetDiffuse(L"Tex_Default");
		return;
	}

	normalSrvHeapIndex = texture->GetSRVHeapIndex();
	normalTextureName = UniversalUtils::ToString(texture->GetNameW());
}

void Material::SetNormal(wstring textureName)
{
	normalSrvHeapIndex = 0;
	normalTextureName = UniversalUtils::ToString(textureName);
}
