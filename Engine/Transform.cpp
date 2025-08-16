#include "pch.h"
#include "Transform.h"

Transform::Transform() : Super(ComponentType::Transform)
{
	_localPosition = { 0.0f, 0.0f, 0.0f };
	_localRotation = { 0.0f, 0.0f, 0.0f };
	_localScale = { 1.0f, 1.0f, 1.0f };

	_localQuaternion = { 0.0f, 0.0f, 0.0f, 1.0f };
	_quaternion = { 0.0f, 0.0f, 0.0f, 1.0f };

	_parent = nullptr;

	_isDirty = true;
	UpdateTransform();
}

Transform::~Transform()
{

}

void Transform::Update()
{

}

void Transform::Render()
{

}

void Transform::ForceUpdateTransform()
{
	XMMATRIX matScale = XMMatrixScaling(_localScale.x, _localScale.y, _localScale.z);
	XMMATRIX matRotation = XMMatrixRotationQuaternion(XMLoadFloat4(&_localQuaternion));
	XMMATRIX matTranslation = XMMatrixTranslation(_localPosition.x, _localPosition.y, _localPosition.z);

	XMMATRIX matTransform = matScale * matRotation * matTranslation;

	XMStoreFloat4x4(&_matLocal, matTransform);

	if (HasParent())
	{
		XMMATRIX parentWorld = XMLoadFloat4x4(&_parent->GetWorldMatrix());
		XMStoreFloat4x4(&_matWorld, matTransform * parentWorld);
	}
	else
	{
		_matWorld = _matLocal;
	}

	_isDirty = false;

	XMVECTOR scale;
	XMVECTOR quaternion;
	XMVECTOR position;
	XMMatrixDecompose(
		&scale,
		&quaternion,
		&position,
		XMLoadFloat4x4(&_matWorld));

	XMStoreFloat3(&_scale, scale);
	_quaternion = quaternion;
	_rotation = MathHelper::ConvertQuaternionToEuler(quaternion);
	XMStoreFloat3(&_position, position);

	for (auto& child : _childs)
	{
		child->ForceUpdateTransform();
	}
}

void Transform::UpdateTransform()
{
	// 앞에서 조건 다 걸러내긴 하는데 혹시 몰라서 한번 더 확인
	if (!_isDirty)
		return;

	XMMATRIX matScale = XMMatrixScaling(_localScale.x, _localScale.y, _localScale.z);
	XMMATRIX matRotation = XMMatrixRotationQuaternion(XMLoadFloat4(&_localQuaternion));
	XMMATRIX matTranslation = XMMatrixTranslation(_localPosition.x, _localPosition.y, _localPosition.z);

	XMMATRIX matTransform = matScale * matRotation * matTranslation;

	XMStoreFloat4x4(&_matLocal, matTransform);

	if (HasParent())
	{
		XMMATRIX parentWorld = XMLoadFloat4x4(&_parent->GetWorldMatrix());
		XMStoreFloat4x4(&_matWorld, matTransform * parentWorld);
	}
	else
	{
		_matWorld = _matLocal;
	}

	_isDirty = false;

	XMVECTOR scale;
	XMVECTOR quaternion;
	XMVECTOR position;
	XMMatrixDecompose(
		&scale,
		&quaternion, 
		&position,
		XMLoadFloat4x4(&_matWorld));

	XMStoreFloat3(&_scale, scale);
	_quaternion = quaternion;
	_rotation = MathHelper::ConvertQuaternionToEuler(quaternion);
	XMStoreFloat3(&_position, position);

	for (auto& child : _childs)
	{
		child->SetDirtyFlag();
	}
}

void Transform::SetLocalRotationRadian(const Vector3& rotation)
{
	_localRotation = rotation;

	XMStoreFloat4(&_localQuaternion, XMQuaternionNormalize(XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z)));
	SetDirtyFlag();
}

void Transform::SetQuaternion(const Vector4& quaternion)
{
	_quaternion = XMQuaternionNormalize(XMLoadFloat4(&quaternion));
	XMVECTOR qLocal;
	if (HasParent())
	{
		XMMATRIX parentRotMat = _parent->GetRotationMatrix();
		XMVECTOR qParent = XMQuaternionRotationMatrix(parentRotMat);
		qLocal = XMQuaternionMultiply(XMLoadFloat4(&quaternion), XMQuaternionInverse(qParent));
	}

	SetLocalQuaternion(qLocal);
}

void Transform::SetQuaternion(const XMVECTOR& quaternion)
{
	_quaternion = XMQuaternionNormalize(quaternion);
	XMVECTOR qLocal;
	if (HasParent())
	{
		XMMATRIX parentRotMat = _parent->GetRotationMatrix();
		XMVECTOR qParent = XMQuaternionRotationMatrix(parentRotMat);
		qLocal = XMQuaternionMultiply(quaternion, XMQuaternionInverse(qParent));
	}

	SetLocalQuaternion(qLocal);
}

void Transform::SetLocalQuaternion(const Vector4& quaternion)
{
	XMStoreFloat4(&_localQuaternion, XMQuaternionNormalize(XMLoadFloat4(&quaternion)));
	SetDirtyFlag();
}

void Transform::SetLocalQuaternion(const XMVECTOR& quaternion)
{
	XMStoreFloat4(&_localQuaternion, XMQuaternionNormalize(quaternion));
	SetDirtyFlag();
}

void Transform::SetPosition(const Vector3& worldPosition)
{
	if (HasParent())
	{
		XMMATRIX inverseWorldMat = XMMatrixInverse(nullptr, XMLoadFloat4x4(&_parent->GetWorldMatrix()));
		Vector3 position;
		XMStoreFloat3(&position, XMVector3Transform(XMLoadFloat3(&worldPosition), inverseWorldMat));

		SetLocalPosition(position);
	}
	else
	{
		SetLocalPosition(worldPosition);
	}
}

void Transform::SetRotationRadian(const Vector3& worldRotation)
{
	XMVECTOR qWorld = XMQuaternionRotationRollPitchYaw(
		worldRotation.x,
		worldRotation.y,
		worldRotation.z);

	Vector3 localEuler = worldRotation;
	if (HasParent())
	{
		XMMATRIX parentRotMat = _parent->GetRotationMatrix();
		XMVECTOR qParent = XMQuaternionRotationMatrix(parentRotMat);
		XMVECTOR qLocal = XMQuaternionMultiply(qWorld, XMQuaternionInverse(qParent));
		localEuler = MathHelper::ConvertQuaternionToEuler(qLocal);
	}

	SetLocalRotationRadian(localEuler);
}

void Transform::SetScale(const Vector3& worldScale)
{
	if (HasParent())
	{
		Vector3 parentScale = _parent->GetScale();
		Vector3 scale = worldScale;
		scale.x /= parentScale.x;
		scale.y /= parentScale.y;
		scale.z /= parentScale.z;

		SetLocalScale(scale);
	}
	else
	{
		SetLocalScale(worldScale);
	}
}

XMMATRIX Transform::GetRotationMatrix()
{
	if (_isDirty)
		UpdateTransform();

	return XMMatrixRotationQuaternion(XMLoadFloat4(&_quaternion));
}

Vector3 Transform::GetRight()
{
	if (_isDirty)
		UpdateTransform();

	return Vector3(-_matLocal._11, -_matLocal._21, _matLocal._31);
}

Vector3 Transform::GetLeft()
{
	if (_isDirty)
		UpdateTransform();

	return Vector3(_matLocal._11, _matLocal._21, -_matLocal._31);
}

Vector3 Transform::GetUp()
{
	if (_isDirty)
		UpdateTransform();

	return Vector3(_matLocal._12, _matLocal._22, -_matLocal._32);
}

Vector3 Transform::GetDown()
{
	if (_isDirty)
		UpdateTransform();

	return Vector3(-_matLocal._12, -_matLocal._22, _matLocal._32);
}

Vector3 Transform::GetLook()
{
	if (_isDirty)
		UpdateTransform();

	return Vector3(-_matLocal._13, -_matLocal._23, _matLocal._33);
}

Vector3 Transform::GetBack()
{
	if (_isDirty)
		UpdateTransform();

	return Vector3(_matLocal._13, _matLocal._23, -_matLocal._33);
}

void Transform::Translate(const Vector3& moveVec)
{
	_localPosition = _localPosition + moveVec;

	SetDirtyFlag();
}

void Transform::Rotate(const Vector3& angle)
{
	XMVECTOR currentQuat = XMLoadFloat4(&_localQuaternion);
	XMVECTOR deltaQuat = XMQuaternionRotationRollPitchYaw(angle.x, angle.y, angle.z);
	XMVECTOR newQuat = XMQuaternionNormalize(XMQuaternionMultiply(currentQuat, deltaQuat));

	_localQuaternion = newQuat;
	_localRotation = MathHelper::ConvertQuaternionToEuler(newQuat);

	SetDirtyFlag();
}

void Transform::Rotate(const XMVECTOR& angle)
{
	Vector3 f3_angle;
	XMStoreFloat3(&f3_angle, angle);
	Rotate(f3_angle);
}

void Transform::Rotate(const Vector4& quat)
{
	XMVECTOR currentQuat = XMLoadFloat4(&_localQuaternion);
	XMVECTOR deltaQuat = XMLoadFloat4(&quat);
	XMVECTOR newQuat = XMQuaternionNormalize(XMQuaternionMultiply(currentQuat, deltaQuat));

	_localQuaternion = newQuat;
	_localRotation = MathHelper::ConvertQuaternionToEuler(newQuat);

	SetDirtyFlag();
}

void Transform::LookAt(const Vector3& targetPos)
{
	XMVECTOR targetVec = XMVector3Normalize(XMLoadFloat3(&(targetPos - GetPosition())));

	XMVECTOR worldUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR sideVec = XMVector3Normalize(XMVector3Cross(worldUp, targetVec));
	XMVECTOR upVec = XMVector3Normalize(XMVector3Cross(targetVec, sideVec));

	if (upVec.m128_f32[1] < 0.0f)
		upVec = -upVec;

	XMMATRIX rotMat = XMMatrixLookToLH(XMLoadFloat3(&GetPosition()), targetVec, upVec);

	rotMat = XMMatrixTranspose(rotMat);
	XMVECTOR quat = XMQuaternionNormalize(XMQuaternionRotationMatrix(rotMat));

	Vector3 euler = MathHelper::ConvertQuaternionToEuler(quat);
	Vector4 quatVec;
	XMStoreFloat4(&quatVec, quat);
	SetLocalQuaternion(quatVec);

	SetDirtyFlag();
}

void Transform::LookAtWithNoRoll(const Vector3& targetPos)
{
	Vector3 dir = targetPos - GetPosition();
	dir = dir.Normalize();

	float pitch = -asin(dir.y);
	float yaw = atan2(dir.x, dir.z);
	float roll = 0.0f;

	XMVECTOR quatPitch = XMQuaternionRotationAxis(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), pitch);
	XMVECTOR quatYaw = XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), yaw);
	XMVECTOR quatFinal = XMQuaternionNormalize(XMQuaternionMultiply(quatYaw, quatPitch));

	SetLocalQuaternion(quatFinal);

	SetDirtyFlag();
}

void Transform::SetLocalMatrix(XMFLOAT4X4 mat)
{
	_matLocal = mat;

	XMVECTOR scale;
	XMVECTOR quaternion;
	XMVECTOR position;
	XMMatrixDecompose(
		&scale,
		&quaternion,
		&position,
		XMLoadFloat4x4(&_matLocal));

	XMStoreFloat3(&_localScale, scale);
	_localQuaternion = quaternion;
	_localRotation = MathHelper::ConvertQuaternionToEuler(quaternion);
	XMStoreFloat3(&_localPosition, position);

	SetDirtyFlag();
}

void Transform::SetWorldMatrix(XMFLOAT4X4 mat)
{
	_matWorld = mat;

	if (HasParent())
		XMStoreFloat4x4(&_matLocal, XMLoadFloat4x4(&_matWorld) * XMMatrixInverse(nullptr, XMLoadFloat4x4(&_parent->GetWorldMatrix())));
	else
		_matLocal = _matWorld;

	SetLocalMatrix(_matLocal);
}

XMFLOAT4X4 Transform::GetLocalMatrix()
{
	if (_isDirty)
		UpdateTransform();

	return _matLocal;
}

XMFLOAT4X4 Transform::GetWorldMatrix()
{
	if (HasParent())
	{
		XMMATRIX parent = XMLoadFloat4x4(&_parent->GetWorldMatrix());

		if (_isDirty)
			UpdateTransform();

		XMMATRIX local = XMLoadFloat4x4(&_matLocal);
		XMFLOAT4X4 result;
		XMStoreFloat4x4(&result, local * parent);
		return result;
	}
	else
	{
		if (_isDirty)
			UpdateTransform();

		return _matWorld;
	}
}

void Transform::SetParent(shared_ptr<Transform> parent)
{
	_parent = parent;

	if (HasParent())
		XMStoreFloat4x4(&_matLocal, XMLoadFloat4x4(&_matWorld) * XMMatrixInverse(nullptr, XMLoadFloat4x4(&_parent->GetWorldMatrix())));
	else
		_matLocal = _matWorld;

	_parent->AddChild(static_pointer_cast<Transform>(shared_from_this()));

	SetLocalMatrix(_matLocal);
}

void Transform::AddChild(shared_ptr<Transform> child)
{
	_childs.push_back(child);
}

void Transform::SetDirtyFlag()
{
	_isDirty = true;
	GetGameObject()->SetFramesDirty();

	for (auto& child : _childs)
		child->SetDirtyFlag();
}
