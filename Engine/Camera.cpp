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
	_viewportSize = { GRAPHIC->GetViewport().Width, GRAPHIC->GetViewport().Height };

	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, _aspectRatio, 1.0f, 1000.0f);
	XMStoreFloat4x4(&_matProj, P);

	XMMATRIX O = XMMatrixOrthographicLH(_viewportSize.x, _viewportSize.y, 0.0f, 1000.0f);
	XMStoreFloat4x4(&_matOrtho, O);
}

void Camera::Update()
{
	if (GetGameObject()->GetFramesDirty() > 0) {
		XMVECTOR eyePos = XMLoadFloat3(&GetTransform()->GetPosition());
		XMVECTOR targetPos = eyePos + XMLoadFloat3(&GetTransform()->GetLook());
		XMVECTOR upVec = XMLoadFloat3(&GetTransform()->GetUp());

		// View
		XMMATRIX matView = XMMatrixLookAtLH(eyePos, targetPos, upVec);
		XMStoreFloat4x4(&_matView, matView);

		// Proj
		if (_aspectRatio != GRAPHIC->GetAspectRatio())
		{
			_aspectRatio = GRAPHIC->GetAspectRatio();
			XMMATRIX matProj = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, _aspectRatio, 1.0f, 1000.0f);
			XMStoreFloat4x4(&_matProj, matProj);
		}

		// View * Proj
		XMStoreFloat4x4(&_matViewProj, matView * XMLoadFloat4x4(&_matProj));
	}

	D3D12_VIEWPORT viewport = GRAPHIC->GetViewport();
	if (_viewportSize.x != viewport.Width || _viewportSize.y != viewport.Height) {
		XMMATRIX O = XMMatrixOrthographicLH(viewport.Width, viewport.Height, 0.0f, 1000.0f);
		XMStoreFloat4x4(&_matOrtho, O);
	}
}

void Camera::OnDestroy()
{
	cout << "OnDestroy - Camera:" << _id << "\n";

	_currentCamera = nullptr;
}
