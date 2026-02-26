#include "pch.h"
#include "Camera.h"

Camera* Camera::_currentCamera = nullptr;
UINT Camera::_cameraCount = 0;

Camera::Camera() : Super(ComponentType::Camera)
{
	_cameraCount++;
	if (_currentCamera == nullptr)
	{
		_currentCamera = this;
	}
}

Camera::~Camera()
{
	cout << "Released - Camera:" << _id << "\n";
}

void Camera::Init()
{
	_aspectRatio = GRAPHIC->GetAspectRatio();
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, _aspectRatio, 1.0f, 1000.0f);
	XMStoreFloat4x4(&_matProj, P);
}

void Camera::Update()
{
	if (GetGameObject()->GetFramesDirty() <= 0)
		return;

	XMVECTOR eyePos = XMLoadFloat3(&GetTransform()->GetPosition());
	XMVECTOR targetPos = eyePos + XMLoadFloat3(&GetTransform()->GetLook());
	XMVECTOR upVec = XMLoadFloat3(&GetTransform()->GetUp());

	XMMATRIX matView = XMMatrixLookAtLH(eyePos, targetPos, upVec);
	XMStoreFloat4x4(&_matView, matView);

	if (_aspectRatio != GRAPHIC->GetAspectRatio())
	{
		_aspectRatio = GRAPHIC->GetAspectRatio();
		XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, _aspectRatio, 1.0f, 1000.0f);
		XMStoreFloat4x4(&_matProj, P);
	}

	XMMATRIX O = XMMatrixOrthographicLH(1920, 1080, 0.0f, 1000.0f);
	XMStoreFloat4x4(&_matOrtho, O);
}

void Camera::OnDestroy()
{
	cout << "OnDestroy - Camera:" << _id << "\n";

	_currentCamera = nullptr;
}
