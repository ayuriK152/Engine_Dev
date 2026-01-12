#include "pch.h"
#include "EditorCamera.h"

void EditorCamera::Init()
{
	_transform = GetTransform();
}

void EditorCamera::Update()
{
	if (INPUTM->IsMouseRightButtonPress())
	{
		Vector2 mouseDelta = INPUTM->GetMouseDelta();

		_yaw += mouseDelta.x * 0.003f;
		_pitch += mouseDelta.y * 0.003f;

		XMVECTOR quatPitch = XMQuaternionRotationAxis(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), _pitch);
		XMVECTOR quatYaw = XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), _yaw);
		XMVECTOR quatFinal = XMQuaternionNormalize(XMQuaternionMultiply(quatYaw, quatPitch));

		_transform->SetLocalQuaternion(quatFinal);

		if (INPUTM->IsKeyPress(KeyValue::W))
			_transform->Translate(_transform->GetLook() * TIME->DeltaTime() * 5.0f);
		if (INPUTM->IsKeyPress(KeyValue::S))
			_transform->Translate(_transform->GetLook() * TIME->DeltaTime() * -5.0f);
		if (INPUTM->IsKeyPress(KeyValue::D))
			_transform->Translate(_transform->GetRight() * TIME->DeltaTime() * 5.0f);
		if (INPUTM->IsKeyPress(KeyValue::A))
			_transform->Translate(_transform->GetRight() * TIME->DeltaTime() * -5.0f);
	}
}
