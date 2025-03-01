#include "pch.h"
#include "Transform.h"

Transform::Transform() : Super(ComponentType::Transform)
{
	_localPosition = { 0.0f, 0.0f, 0.0f };
	_localRotation = { 0.0f, 0.0f, 0.0f };
	_localScale = { 1.0f, 1.0f, 1.0f };

	_parent = nullptr;

	UpdateTransform();
}

Transform::~Transform()
{

}

void Transform::UpdateTransform()
{
	XMMATRIX matScale = XMMatrixScaling(_localScale.x, _localScale.y, _localScale.z);
	XMMATRIX matRotation = XMMatrixRotationX(_localRotation.x);
	matRotation *= XMMatrixRotationY(_localRotation.y);
	matRotation *= XMMatrixRotationZ(_localRotation.z);
	XMMATRIX matTranslation = XMMatrixTranslation(_localPosition.x, _localPosition.y, _localPosition.z);

	XMStoreFloat4x4(&_matLocal, matScale * matRotation * matTranslation);

	if (HasParent())
	{
		XMMATRIX matWorld = XMLoadFloat4x4(&_matLocal) * XMLoadFloat4x4(&_parent->GetWorldMatrix());
		XMStoreFloat4x4(&_matWorld, matWorld);
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
}

void Transform::SetPosition(const Vector3& worldPosition)
{
	if (HasParent())
	{
		XMMATRIX inverseWorldMat = XMMatrixInverse(nullptr, XMLoadFloat4x4(&_parent->GetWorldMatrix()));
		Vector3 position;
		XMStoreFloat3(&position, XMVector3Transform(XMLoadFloat3(&worldPosition), inverseWorldMat));
	}
	else
	{
		SetLocalPosition(worldPosition);
	}
}

void Transform::SetRotationRadian(const Vector3& worldRotation)
{
	if (HasParent())
	{
		XMMATRIX inverseWorldMat = XMMatrixInverse(nullptr, XMLoadFloat4x4(&_parent->GetWorldMatrix()));
		Vector3 rotation;
		XMStoreFloat3(&rotation, XMVector3TransformNormal(XMLoadFloat3(&worldRotation), inverseWorldMat));
		
		SetLocalRotationRadian(rotation);
	}
	else
	{
		SetLocalRotationRadian(worldRotation);
	}
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

Vector3 Transform::GetRight()
{
	Vector3 right(-_matWorld._11, -_matWorld._21, _matWorld._31);
	return right;
}

Vector3 Transform::GetLeft()
{
	Vector3 left(_matWorld._11, _matWorld._21, -_matWorld._31);
	return left;
}

Vector3 Transform::GetUp()
{
	Vector3 up(_matWorld._12, _matWorld._22, -_matWorld._32);
	return up;
}

Vector3 Transform::GetDown()
{
	Vector3 down(-_matWorld._12, -_matWorld._22, _matWorld._32);
	return down;
}

Vector3 Transform::GetLook()
{
	Vector3 look(-_matWorld._13, -_matWorld._23, _matWorld._33);
	return look;
}

Vector3 Transform::GetBack()
{
	Vector3 look(_matWorld._13, _matWorld._23, -_matWorld._33);
	return look;
}

void Transform::Translate(const Vector3& moveVec)
{
	_localPosition = MathHelper::VectorAddition(_localPosition, moveVec);
	UpdateTransform();
	GetGameObject()->numFramesDirty++;
}

// 반드시 수정이 필요함
void Transform::Rotate(const Vector3& angle)
{
	_localRotation.x += XMConvertToRadians(angle.x);
	_localRotation.y += XMConvertToRadians(angle.y);
	_localRotation.z += XMConvertToRadians(angle.z);
	UpdateTransform();
	GetGameObject()->numFramesDirty++;
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
}

XMFLOAT4X4 Transform::GetWorldMatrix()
{
	if (HasParent())
		return _parent->GetWorldMatrix();
	else
		return _matWorld;
}
