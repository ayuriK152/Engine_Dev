#include "pch.h"
#include "Light.h"

Light::Light() : Component(ComponentType::Light)
{
	_numFramesDirty = RENDER->GetNumFrameResources();
}

Light::Light(XMFLOAT4 diffuse) : Component(ComponentType::Light), diffuse(diffuse)
{
	_numFramesDirty = RENDER->GetNumFrameResources();
}

Light::~Light()
{
	cout << "Released - Light:" << _id << "\n";
}

void Light::SetFramesDirty()
{
	_numFramesDirty = RENDER->GetNumFrameResources();
}

