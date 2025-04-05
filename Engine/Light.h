#pragma once
#include "Component.h"

class Light : public Component
{
	using Super = Component;
public:
	Light();
	Light(XMFLOAT4 ambient, XMFLOAT4 diffuse, XMFLOAT4 specular, XMFLOAT4 emissive);
	virtual ~Light();

	virtual void Init() = 0;
	virtual void Update() = 0;

public:
	virtual LightConstants GetLightConstants() = 0;

public:
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular;
	XMFLOAT4 emissive;

	int numFramesDirty = 0;
};

