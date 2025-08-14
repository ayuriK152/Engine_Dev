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
	}
}

void TPVCamera::Update()
{
	if (cameraTransform == nullptr || targetTransform == nullptr)
		return;

	GetTransform()->SetPosition(targetTransform->GetPosition() + offset);

	if ((cameraTransform->GetLocalPosition() - GetTransform()->GetLocalPosition()).Length() != distance)
		cameraTransform->SetLocalPosition(Vector3(0.0f, 0.0f, -distance));

	Vector3 rotation(INPUTM->GetMouseDelta().y, INPUTM->GetMouseDelta().x, 0.0f);
	rotation = rotation * rotationSpeed * TIME->DeltaTime();
	GetTransform()->Rotate(rotation);

	cameraTransform->LookAtWithNoRoll(targetTransform->GetPosition());
}
