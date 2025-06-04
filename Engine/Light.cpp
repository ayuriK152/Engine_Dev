#include "pch.h"
#include "Light.h"

Light::Light() : Component(ComponentType::Light)
{
	ambient = {};

	_numFramesDirty = GRAPHIC->GetNumFrameResources();
}

Light::Light(
	XMFLOAT4 ambient, 
	XMFLOAT4 diffuse, 
	XMFLOAT4 specular) :
	Component(ComponentType::Light),
	ambient(ambient),
	diffuse(diffuse),
	specular(specular)
{
	_numFramesDirty = GRAPHIC->GetNumFrameResources();
}

Light::~Light()
{

}

void Light::SetFramesDirty()
{
	_numFramesDirty = GRAPHIC->GetNumFrameResources();
}

