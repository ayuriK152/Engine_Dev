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
	XMFLOAT4 specular) :
	Component(ComponentType::Light),
	ambient(ambient),
	diffuse(diffuse),
	specular(specular)
{
	numFramesDirty = GRAPHIC->GetNumFrameResources();
}

Light::~Light()
{

}

// ���ʿ��ϰ� ������ ���� �þ�� �� ������
void Light::IncreaseNumFramesDirty()
{
	if (numFramesDirty < GRAPHIC->GetNumFrameResources())
		numFramesDirty++;
}

