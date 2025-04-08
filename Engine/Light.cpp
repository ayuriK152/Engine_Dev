#include "pch.h"
#include "Light.h"

Light::Light() : Component(ComponentType::Light)
{
	ambient = {};

	numFramesDirty = GRAPHIC->GetNumFrameResources();
}

Light::Light(
	XMFLOAT4 ambient, 
	XMFLOAT4 diffuse, 
	XMFLOAT4 specular, 
	XMFLOAT4 emissive) :
	Component(ComponentType::Light),
	ambient(ambient),
	diffuse(diffuse),
	specular(specular),
	emissive(emissive)
{
	numFramesDirty = GRAPHIC->GetNumFrameResources();
}

Light::~Light()
{

}

// 불필요하게 데이터 값이 늘어나는 것 방지용
void Light::IncreaseNumFramesDirty()
{
	if (numFramesDirty < GRAPHIC->GetNumFrameResources())
		numFramesDirty++;
}

