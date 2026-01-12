#pragma once
#include "Script.h"
class EditorCamera : public Script
{
public:
	void Init() override;
	void Update() override;

private:
	shared_ptr<Transform> _transform;
	float _yaw = 0.0f;
	float _pitch = 0.0f;
};

