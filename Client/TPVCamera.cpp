#include "pch.h"
#include "TPVCamera.h"

void TPVCamera::Init()
{
	if (cameraTransform == nullptr) {
		DEBUG->ErrorLog("Camera Transform was nullptr!");
		return;
	}
	if (armTransform == nullptr) {
		DEBUG->ErrorLog("Camera arm Transform was nullptr!");
		return;
	}

	_transform = GetTransform();
	armTransform->SetLocalPosition(Vector3(0.0f, 0.0f, 0.0f));

	if (cameraTransform->GetParent() != _transform) {
		cameraTransform->SetParent(armTransform);
		cameraTransform->SetLocalPosition(Vector3(0.0f, 0.0f, -distance));
	}

	if (onwerTransform != nullptr) {
		_pivotPosition = onwerTransform->GetPosition() + offset;
		_transform->SetPosition(_pivotPosition);
		cameraTransform->LookAtWithNoRoll(_pivotPosition);
	}
}

void TPVCamera::Update()
{
	if (!isCameraControllOn || cameraTransform == nullptr || onwerTransform == nullptr)
		return;

	if (onwerTransform->GetGameObject()->GetFramesDirty() > 0) {
		_pivotPosition = onwerTransform->GetPosition() + offset;
		_transform->SetPosition(_pivotPosition);
	}

	if (!isLockOn) {
		float deltaX = INPUTM->GetMouseDelta().x * sensitivity * TIME->DeltaTime();
		float deltaY = INPUTM->GetMouseDelta().y * sensitivity * TIME->DeltaTime();

		if (abs(deltaX) > 0.0f)
			_transform->Rotate(Vector3(0.0f, deltaX, 0.0f));
		if (abs(deltaY) > 0.0f) {
			_pitch += deltaY * 50.0f;
			_pitch = clamp(_pitch, pitchLimit.y, pitchLimit.x);
			armTransform->SetLocalRotation(Vector3(_pitch, 0.0f, 0.0f));
		}

		cameraTransform->LookAtWithNoRoll(_pivotPosition);
	}
	else if (lockOnTargetTransform != nullptr) {
		Vector3 targetPosition = lockOnTargetTransform->GetPosition();
		_transform->LookAtOnlyYaw(targetPosition);	// yÃà È¸Àü

		Vector3 pivotToTargetVec = (targetPosition - _pivotPosition).Normalize();
		Vector3 pivotToCameraVec = (cameraTransform->GetPosition() - _pivotPosition).Normalize();
		float pitchDelta = acos(pivotToCameraVec.Dot(-pivotToTargetVec) / (pivotToCameraVec.Length() * pivotToTargetVec.Length())) * 180.0f / MathHelper::Pi;
		if (pitchDelta > 0.0f) {
			_pitch = pivotToTargetVec.y > -pivotToCameraVec.y ? _pitch - pitchDelta : _pitch + pitchDelta;
			_pitch = clamp(_pitch, pitchLimit.y, pitchLimit.x);
			armTransform->SetLocalRotation(Vector3(_pitch, 0.0f, 0.0f));
		}
		
		cameraTransform->LookAtWithNoRoll(lockOnTargetTransform->GetPosition());
	}
}
