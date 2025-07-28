#include "pch.h"
#include "Transform.h"

Transform::Transform() : Super(ComponentType::Transform)
{
	_localPosition = { 0.0f, 0.0f, 0.0f };
	_localRotation = { 0.0f, 0.0f, 0.0f };
	_localScale = { 1.0f, 1.0f, 1.0f };

	_quaternion = { 0.0f, 0.0f, 0.0f ,1.0f };

	_parent = nullptr;

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

void Transform::UpdateTransform()
{
	XMMATRIX matScale = XMMatrixScaling(_localScale.x, _localScale.y, _localScale.z);
	XMMATRIX matRotation = XMMatrixRotationQuaternion(XMLoadFloat4(&_quaternion));
	XMMATRIX matTranslation = XMMatrixTranslation(_localPosition.x, _localPosition.y, _localPosition.z);

	XMStoreFloat4x4(&_matLocal, matScale * matRotation * matTranslation);

	if (HasParent())
	{
		XMMATRIX parentWorld = XMLoadFloat4x4(&_parent->GetWorldMatrix());
		XMStoreFloat4x4(&_matWorld, matScale * matRotation * matTranslation * parentWorld);
	}
	else
	{
		_matWorld = _matLocal;
	}

	XMVECTOR scale;
	XMVECTOR quaternion;
	XMVECTOR position;
	XMMatrixDecompose(
		&scale,
		&quaternion, 
		&position,
		XMLoadFloat4x4(&_matWorld));

	XMStoreFloat3(&_scale, scale);
	_rotation = MathHelper::ConvertQuaternionToEuler(quaternion);
	XMStoreFloat3(&_position, position);

	for (auto& child : _childs)
	{
		child->UpdateTransform();
		child->GetGameObject()->SetFramesDirty();
	}
}

void Transform::SetLocalRotationRadian(const Vector3& rotation)
{
	_localRotation = rotation;

	XMStoreFloat4(&_quaternion, XMQuaternionNormalize(XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z)));

	UpdateTransform();
	GetGameObject()->SetFramesDirty();
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

		// 로컬 회전 = 부모^-1 * 월드
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
	return XMMatrixRotationQuaternion(XMLoadFloat4(&_quaternion));
}

Vector3 Transform::GetRight()
{
	Vector3 right(-_matLocal._11, -_matLocal._21, _matLocal._31);
	return right;
}

Vector3 Transform::GetLeft()
{
	Vector3 left(_matLocal._11, _matLocal._21, -_matLocal._31);
	return left;
}

Vector3 Transform::GetUp()
{
	Vector3 up(_matLocal._12, _matLocal._22, -_matLocal._32);
	return up;
}

Vector3 Transform::GetDown()
{
	Vector3 down(-_matLocal._12, -_matLocal._22, _matLocal._32);
	return down;
}

Vector3 Transform::GetLook()
{
	Vector3 look(-_matLocal._13, -_matLocal._23, _matLocal._33);
	return look;
}

Vector3 Transform::GetBack()
{
	Vector3 look(_matLocal._13, _matLocal._23, -_matLocal._33);
	return look;
}

void Transform::Translate(const Vector3& moveVec)
{
	_localPosition = MathHelper::VectorAddition(_localPosition, moveVec);
	UpdateTransform();
	GetGameObject()->SetFramesDirty();
}

void Transform::Rotate(const Vector3& angle)
{
	XMVECTOR currentQuat = XMLoadFloat4(&_quaternion);
	XMVECTOR deltaQuat = XMQuaternionRotationRollPitchYaw(angle.x, angle.y, angle.z);
	XMVECTOR newQuat = XMQuaternionNormalize(XMQuaternionMultiply(currentQuat, deltaQuat));

	_quaternion = { newQuat.m128_f32[0], newQuat.m128_f32[1], newQuat.m128_f32[2], newQuat.m128_f32[3] };
	_localRotation = MathHelper::ConvertQuaternionToEuler(newQuat);

	UpdateTransform();
	GetGameObject()->SetFramesDirty();
}

void Transform::Rotate(const XMVECTOR& angle)
{
	XMFLOAT3 f3_angle;
	XMStoreFloat3(&f3_angle, angle);
	Rotate(f3_angle);
}

void Transform::LookAt(const Vector3& targetPos)
{
	XMVECTOR targetVec = XMLoadFloat3(&MathHelper::VectorSubtract(targetPos, _position));
	
	XMVECTOR sideVec = XMVector3Normalize(XMVector3Cross(targetVec, XMVECTOR({ 0.0f, 1.0f, 0.0f })));
	XMVECTOR upVec = XMVector3Normalize(XMVector3Cross(targetVec, sideVec));

	if (upVec.m128_f32[1] < 0.0f)
		upVec = -upVec;

	XMMATRIX viewMat = XMMatrixLookAtLH(XMLoadFloat3(&_position), XMLoadFloat3(&targetPos), upVec);
	
	viewMat = XMMatrixTranspose(viewMat);
	XMVECTOR quat = XMQuaternionRotationMatrix(viewMat);

	Vector3 euler = MathHelper::ConvertQuaternionToEuler(quat);
	SetRotationRadian(euler);
	UpdateTransform();
	GetGameObject()->SetFramesDirty();
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
	_localRotation = MathHelper::ConvertQuaternionToEuler(quaternion);
	XMStoreFloat3(&_localPosition, position);

	UpdateTransform();
	GetGameObject()->SetFramesDirty();
}

void Transform::SetObjectWorldMatrix(XMFLOAT4X4 mat)
{
	_matWorld = mat;

	if (HasParent())
		XMStoreFloat4x4(&_matLocal, XMLoadFloat4x4(&_matWorld) * XMMatrixInverse(nullptr, XMLoadFloat4x4(&_parent->GetWorldMatrix())));
	else
		_matLocal = _matWorld;

	SetLocalMatrix(_matLocal);
}

XMFLOAT4X4 Transform::GetWorldMatrix()
{
	if (HasParent())
	{
		XMMATRIX parent = XMLoadFloat4x4(&_parent->GetWorldMatrix());
		XMMATRIX local = XMLoadFloat4x4(&_matLocal);
		XMFLOAT4X4 result;
		XMStoreFloat4x4(&result, local * parent);
		return result;
	}
	else
		return _matWorld;
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

//bool Transform::RemoveChild(shared_ptr<GameObject> object)
//{
//
//}
