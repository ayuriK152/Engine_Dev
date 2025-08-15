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
}

void TPVCamera::Update()
{
	if (targetTransform == nullptr)
		return;

	GetTransform()->SetPosition(targetTransform->GetPosition() + offset);

	if (!isCameraControllOn || cameraTransform == nullptr)
		return;

	Vector3 rotation(INPUTM->GetMouseDelta().y, INPUTM->GetMouseDelta().x, 0.0f);
	rotation = rotation * sensitivity * TIME->DeltaTime();
	GetTransform()->Rotate(rotation);

	cameraTransform->LookAtWithNoRoll(targetTransform->GetPosition());
}
