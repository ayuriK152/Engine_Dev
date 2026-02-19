// Third Person View Camera Script

#pragma once

class TPVCamera : public Script
{
public:
	void Init() override;
	void Update() override;

public:
	shared_ptr<Transform> armTransform;
	shared_ptr<Transform> cameraTransform;
	shared_ptr<Transform> onwerTransform;
	shared_ptr<Transform> lockOnTargetTransform;

	float distance = 5.0f;
	Vector3 offset = { 0.0f, 0.0f, 0.0f };
	float sensitivity = 1.0f;
	Vector2 pitchLimit = { 80.0f, -80.0f };
	float rotationSharpness = 0.3f;			// range : 0.1f ~ 1.0f, 사용시 제곱해서 사용해야함.
	float pivotMovementSharpness = 6.0f;	// range : 1.0f ~ 10.0f

	// 카메라 움직임 제어 여부
	bool isCameraControllOn = true;
	bool isLockOn = false;

private:
	shared_ptr<Transform> _transform;
	Vector3 _pivotPosition;
	Vector3 _targetPivotPosition;
	float _pitch = 0.0f;

	shared_ptr<UIPanel> _lockOnMarker;
};

