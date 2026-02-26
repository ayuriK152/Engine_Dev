#pragma once
#include "Script.h"
class EditorCamera : public Script
{
public:
	~EditorCamera();

	void Init() override;
	void Update() override;

	void OnDestroy() override;

private:
	shared_ptr<Transform> _transform;
	float _yaw = 0.0f;
	float _pitch = 0.0f;
};

