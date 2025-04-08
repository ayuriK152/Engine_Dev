#include "pch.h"
#include "DirectionalLight.h"

DirectionalLight::DirectionalLight() : Super()
{
	direction = GetTransform()->GetLook();
}

DirectionalLight::DirectionalLight(
	XMFLOAT4 ambient, 
	XMFLOAT4 diffuse, 
	XMFLOAT4 specular,
	XMFLOAT4 emissive) : 
	Super(ambient, diffuse, specular, emissive)
{

}

DirectionalLight::~DirectionalLight()
{

}

void DirectionalLight::Init()
{
	direction = GetTransform()->GetLook();

	RENDER->AddLight(this);
}

void DirectionalLight::Update()
{
	if (GetGameObject()->numFramesDirty > 0)
	{
		direction = GetTransform()->GetLook();
		IncreaseNumFramesDirty();
	}
}

LightConstants DirectionalLight::GetLightConstants()
{
	LightConstants constants;
	constants.Ambient = ambient;
	constants.Diffuse = diffuse;
	constants.Specular = specular;
	constants.Emissive = emissive;
	constants.Direction = direction;

	return constants;
}
