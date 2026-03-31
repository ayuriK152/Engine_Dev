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
		XMFLOAT4 specular);
	virtual ~DirectionalLight();

	void Init() override;
	void Update() override;

	void OnDestroy() override;

	void LoadXML(XMLElement* compElem) override;
	void SaveXML(XMLElement* compElem) override;

	ComponentSnapshot CaptureSnapshot() override;
	void RestoreSnapshot(ComponentSnapshot snapshot) override;

public:
	LightConstants GetLightConstants() override;

public:
	Bulb::Vector3 direction = { 0.0f, 1.0f, 0.0f };

private:
	shared_ptr<Transform> _transform;
};

