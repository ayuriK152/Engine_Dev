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

XMFLOAT4X4 Transform::GetWorldMatrix()
{
	if (HasParent())
		return _parent->GetWorldMatrix();
	else
		return _matWorld;
}
