#include "pch.h"
#include "EditorCamera.h"

REGISTER_COMPONENT(EditorCamera)

EditorCamera::~EditorCamera()
{

}

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

		Vector3 right = _transform->GetRight();
		Vector3 up = _transform->GetUp();

		//XMVECTOR quatPitch = XMQuaternionRotationAxis(XMQuaternionNormalize(XMVectorSet(right.x, right.y, right.z, 0.0f)), _pitch);
		XMVECTOR quatPitch = XMQuaternionRotationAxis(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), _pitch);
		//XMVECTOR quatYaw = XMQuaternionRotationAxis(XMQuaternionNormalize(XMVectorSet(up.x, up.y, up.z, 0.0f)), _yaw);
		XMVECTOR quatYaw = XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), _yaw);
		//XMVECTOR quatFinal = XMQuaternionNormalize(XMQuaternionMultiply(quatYaw, quatPitch));

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

void EditorCamera::LoadXML(XMLElement* compElem)
{

}

void EditorCamera::SaveXML(XMLElement* compElem)
{
	compElem->SetAttribute("ComponentType", "EditorCamera");
}
