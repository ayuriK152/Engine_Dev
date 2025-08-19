// Third Person View Camera Script

#pragma once

class TPVCamera : public Script
{
public:
	void Init() override;
	void Update() override;

public:
	shared_ptr<Transform> cameraTransform;
	shared_ptr<Transform> targetTransform;
	float distance = 8.0f;
	Vector3 offset;
	float sensitivity = 1.0f;

	// 카메라 움직임 제어 여부
	bool isCameraControllOn = true;

private:
	float _pitch = 0.0f;
};

