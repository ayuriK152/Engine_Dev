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
	XMMATRIX matRotation = XMMatrixRotationX(_localRotation.x / 180.0f * MathHelper::Pi);
	matRotation *= XMMatrixRotationY(_localRotation.y / 180.0f * MathHelper::Pi);
	matRotation *= XMMatrixRotationZ(_localRotation.z / 180.0f * MathHelper::Pi);
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

void Transform::SetRotation(const Vector3& worldRotation)
{
	if (HasParent())
	{
		XMMATRIX inverseWorldMat = XMMatrixInverse(nullptr, XMLoadFloat4x4(&_parent->GetWorldMatrix()));
		Vector3 rotation;
		XMStoreFloat3(&rotation, XMVector3TransformNormal(XMLoadFloat3(&worldRotation), inverseWorldMat));
		
		SetLocalRotation(rotation);
	}
	else
	{
		SetLocalRotation(worldRotation);
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

Vector3 Transform::GetLook()
{
	Vector3 look(_matWorld._13, _matWorld._23, _matWorld._33);
	return look;
}

Vector3 Transform::GetUp()
{
	Vector3 up(_matWorld._12, _matWorld._22, _matWorld._32);
	return up;
}

XMFLOAT4X4 Transform::GetWorldMatrix()
{
	if (HasParent())
		return _parent->GetWorldMatrix();
	else
		return _matWorld;
}
