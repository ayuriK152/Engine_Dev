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

	if (EDITOR->isNoneMouseMode) {
		// Mouse Delta를 사용하지 않고 키보드 화살표로 각도조절할 수 있도록
		if (INPUTM->IsKeyPress(KeyValue::UP)) {
			_pitch -= TIME->DeltaTime();
		}
		if (INPUTM->IsKeyPress(KeyValue::DOWN)) {
			_pitch += TIME->DeltaTime();
		}
		if (INPUTM->IsKeyPress(KeyValue::RIGHT)) {
			_yaw += TIME->DeltaTime();
		}
		if (INPUTM->IsKeyPress(KeyValue::LEFT)) {
			_yaw -= TIME->DeltaTime();
		}

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
		if (INPUTM->IsKeyPress(KeyValue::E))
			_transform->Translate(_transform->GetUp() * TIME->DeltaTime() * 5.0f);
		if (INPUTM->IsKeyPress(KeyValue::Q))
			_transform->Translate(_transform->GetUp() * TIME->DeltaTime() * -5.0f);
	}
	else {
		if (INPUTM->IsMouseRightButtonPress()) {
			Bulb::Vector2 mouseDelta = INPUTM->GetMouseDelta();

			_yaw += mouseDelta.x * TIME->DeltaTime();
			_pitch += mouseDelta.y * TIME->DeltaTime();

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
			if (INPUTM->IsKeyPress(KeyValue::E))
				_transform->Translate(_transform->GetUp() * TIME->DeltaTime() * 5.0f);
			if (INPUTM->IsKeyPress(KeyValue::Q))
				_transform->Translate(_transform->GetUp() * TIME->DeltaTime() * -5.0f);
		}
	}
}

void EditorCamera::OnDestroy()
{
	_transform.reset();
}
