#pragma once
#include "Light.h"

class DirectionalLight : public Light
{
	using Super = Light;
public:
	DirectionalLight();
	DirectionalLight(
		XMFLOAT4 ambient,
		XMFLOAT4 diffuse, 
		XMFLOAT4 specular, 
		XMFLOAT4 emissive);
	virtual ~DirectionalLight();

	void Init() override;
	void Update() override;

public:
	LightConstants GetLightConstants() override;

public:
	Vector3 direction;
};

