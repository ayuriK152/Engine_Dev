#include "pch.h"
#include "TPVCamera.h"

TPVCamera::~TPVCamera()
{
	cout << "asdf";
}

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

	if (rotationSharpness < 0.1f || rotationSharpness > 1.0f) {
		DEBUG->WarnLog("[TPVCamera] - Rotation sharpness value should be between 0.1 to 1.0! Value has been clamped automatically.");
		rotationSharpness = clamp(rotationSharpness, 0.1f, 1.0f);
	}

	if (pivotMovementSharpness < 1.0f || pivotMovementSharpness > 10.0f) {
		DEBUG->WarnLog("[TPVCamera] - Pivot movement sharpness value should be between 1.0 to 10.0! Value has been clamped automatically.");
		pivotMovementSharpness = clamp(pivotMovementSharpness, 1.0f, 10.0f);
	}

	_lockOnMarker = UI->CreateUI<UIPanel>();
	_lockOnMarker->GetTransform()->SetDynamicPosition(true);
	_lockOnMarker->SetTexture(L"LockOnMarker");
	_lockOnMarker->SetSize(Vector2(60.0f, 60.0f));
	_lockOnMarker->SetRenderActive(false);

}

void TPVCamera::Update()
{
	// DEBUG
	if (INPUTM->IsKeyDown(KeyValue::NUM_1))
	{
		INPUTM->SetMouseCenterFixMode(!INPUTM->IsMouseCenterFixed());
		ShowCursor(INPUTM->IsMouseCenterFixed() ? FALSE : TRUE);
		isCameraControllOn = !isCameraControllOn;
	}

	if (!isCameraControllOn || cameraTransform == nullptr || onwerTransform == nullptr)
		return;

	if (onwerTransform->GetGameObject()->GetFramesDirty() > 0) {
		_targetPivotPosition = onwerTransform->GetPosition() + offset;
	}

	if ((_targetPivotPosition - _pivotPosition).Length() > 0.000001f) {
		_pivotPosition = _pivotPosition + (_targetPivotPosition - _pivotPosition) * pivotMovementSharpness * TIME->DeltaTime();
		_transform->SetPosition(_pivotPosition);
	}

	if (!isLockOn) {
		float deltaX = INPUTM->GetMouseDelta().x * sensitivity * TIME->DeltaTime();
		float deltaY = INPUTM->GetMouseDelta().y * sensitivity * TIME->DeltaTime();

		if (_lockOnMarker->IsRenderActive())
			_lockOnMarker->SetRenderActive(false);

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
		_transform->LookAtOnlyYaw(targetPosition, pow(rotationSharpness, 2.0f));	// yÃà È¸Àü

		if (!_lockOnMarker->IsRenderActive())
			_lockOnMarker->SetRenderActive(true);
		_lockOnMarker->GetTransform()->SetPosition(targetPosition);


		Vector3 relTargetPos = targetPosition - _pivotPosition;
		float horizontalDist = Vector2(relTargetPos.x, relTargetPos.z).Length();
		float targetPitch = -atan2(relTargetPos.y, horizontalDist) * (180.0f / MathHelper::Pi);
		if (targetPitch > 0.0f) {
			_pitch = lerp(_pitch, clamp(targetPitch, pitchLimit.y, pitchLimit.x), pow(rotationSharpness, 2.0f));
			armTransform->SetLocalRotation(Vector3(_pitch, 0.0f, 0.0f));
		}
		
		cameraTransform->LookAtWithNoRoll(_pivotPosition);
	}
}

void TPVCamera::OnDestroy()
{
	cout << "OnDestroy - TPVCamera:" << _id << "\n";

	_transform.reset();
	_lockOnMarker.reset();
}
