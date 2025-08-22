#include "pch.h"
#include "TPVCamera.h"

void TPVCamera::Init()
{
	if (cameraTransform == nullptr)
	{
		return;
	}

	if (cameraTransform->GetParent() != GetTransform())
	{
		cameraTransform->SetParent(GetTransform());
		cameraTransform->SetLocalPosition(Vector3(0.0f, 0.0f, -distance));
	}
	if (targetTransform != nullptr)
	{
		GetTransform()->SetPosition(targetTransform->GetPosition() + offset);
		cameraTransform->LookAtWithNoRoll(targetTransform->GetPosition());
	}
}

void TPVCamera::Update()
{
	if (targetTransform == nullptr)
		return;

	GetTransform()->SetPosition(targetTransform->GetPosition() + offset);

	if (!isCameraControllOn || cameraTransform == nullptr)
		return;

	float deltaX = INPUTM->GetMouseDelta().x * sensitivity * TIME->DeltaTime();
	float deltaY = INPUTM->GetMouseDelta().y * sensitivity * TIME->DeltaTime();
	_pitch += deltaY;
	float clampedPitch = clamp(_pitch, XMConvertToRadians(-80.0f), XMConvertToRadians(80.0f));
	// 오차 조금 있을듯. 작동에는 별 문제가 없지만 가능하면 수정 요함.
	if (_pitch != clampedPitch)
	{
		deltaY = 0;
		_pitch = clampedPitch;
	}

	XMVECTOR qy = XMQuaternionRotationAxis(
		XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), deltaX);

	Vector3 right = GetTransform()->GetRight();
	right.y = 0.0f;
	right = right.Normalize();
	XMVECTOR qx = XMQuaternionRotationAxis(XMLoadFloat3(&right), deltaY);

	XMVECTOR currentQuat = XMLoadFloat4(&GetTransform()->GetLocalQuaternion());
	XMVECTOR newQuat = XMQuaternionNormalize(
		XMQuaternionMultiply(currentQuat, XMQuaternionMultiply(qy, qx)));

	GetTransform()->SetLocalQuaternion(newQuat);

	//Vector3 rotation(INPUTM->GetMouseDelta().y, INPUTM->GetMouseDelta().x, 0.0f);
	//rotation = rotation * sensitivity * TIME->DeltaTime();
	//GetTransform()->Rotate(rotation);

	cameraTransform->LookAtWithNoRoll(targetTransform->GetPosition());
}
