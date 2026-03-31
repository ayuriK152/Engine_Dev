#include "pch.h"
#include "EditorCamera.h"

// REGISTER_COMPONENT(EditorCamera)

EditorCamera::~EditorCamera()
{

}

void EditorCamera::Init()
{
	_transform = GetTransform();
}

void EditorCamera::Update()
{
	if (EDITOR->IsOnPlay()) return;

	if (INPUTM->IsMouseRightButtonPress())
	{
		Bulb::Vector2 mouseDelta = INPUTM->GetMouseDelta();

		_yaw += mouseDelta.x * 0.003f;
		_pitch += mouseDelta.y * 0.003f;

		Bulb::Vector3 right = _transform->GetRight();
		Bulb::Vector3 up = _transform->GetUp();

		XMVECTOR quatPitch = XMQuaternionRotationAxis(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), _pitch);
		XMVECTOR quatYaw = XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), _yaw);

		XMVECTOR quatFinal = XMQuaternionRotationRollPitchYaw(_pitch, _yaw, 0.0f);

		_transform->SetQuaternion(quatFinal);

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

void EditorCamera::OnDestroy()
{
	_transform.reset();
}
