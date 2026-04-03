#pragma once

class BULB_API EditorCamera : public Script
{
public:
	~EditorCamera();

	void Init() override;
	void Update() override;

	void OnDestroy() override;

	void LoadXML(Bulb::XMLElement compElem) override {}
	void SaveXML(Bulb::XMLElement compElem) override {}

private:
	shared_ptr<Transform> _transform;
	float _yaw = 0.0f;
	float _pitch = 0.0f;
};

