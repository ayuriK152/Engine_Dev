#include "pch.h"
#include "Camera.h"

XMFLOAT3 Camera::_eyePos(0.0f, 0.0f, 0.0f);
XMFLOAT4X4 Camera::_mainMatView = MathHelper::Identity4x4();
XMFLOAT4X4 Camera::_mainMatProj = MathHelper::Identity4x4();

Camera::Camera() : Super(ComponentType::Camera)
{

}

Camera::~Camera()
{

}

void Camera::Init()
{
	_aspectRatio = GRAPHIC->GetAspectRatio();
}

void Camera::Update()
{
	_eyePos = GetTransform()->GetPosition();
	XMVECTOR eyePos = XMLoadFloat3(&_eyePos);
	XMVECTOR targetPos = eyePos + XMLoadFloat3(&GetTransform()->GetLook());
	XMVECTOR upVec = XMLoadFloat3(&GetTransform()->GetUp());
	cout << "Camera: " << upVec.m128_f32[0] << " " << upVec.m128_f32[1] << " " << upVec.m128_f32[2] << endl << endl;

	XMMATRIX matView = XMMatrixLookAtLH(eyePos, targetPos, upVec);
	XMStoreFloat4x4(&_matView, matView);

	{
		_mainMatView = _matView;
	}

	if (_aspectRatio != GRAPHIC->GetAspectRatio())
	{
		_aspectRatio = GRAPHIC->GetAspectRatio();
		XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, _aspectRatio, 1.0f, 1000.0f);
		XMStoreFloat4x4(&_matProj, P);

		{
			_mainMatProj = _matProj;
		}
	}
}
