#include "pch.h"
#include "Camera.h"

// REGISTER_COMPONENT(Camera);

shared_ptr<Camera> Camera::_currentCamera = nullptr;
shared_ptr<Camera> Camera::_editorCamera = nullptr;
UINT Camera::_cameraCount = 0;

Camera::Camera() : Super(ComponentType::Camera)
{
	_cameraCount++;
}

Camera::~Camera()
{
#ifdef PRINT_DEBUG_CONSOLE_LOG
	cout << "Released - Camera:" << _id << "\n";
#endif
}

void Camera::Init()
{
#ifdef BULB_EDITOR
	if (GetGameObject()->GetTag() == "EditorCamera")
		_editorCamera = static_pointer_cast<Camera>(shared_from_this());
#endif

	if (_isMainCamera)
		SetAsMainCamera();

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
#ifdef PRINT_DEBUG_CONSOLE_LOG
	cout << "OnDestroy - Camera:" << _id << "\n";
#endif

	_currentCamera = nullptr;
}

void Camera::LoadXML(Bulb::XMLElement compElem)
{
	_isMainCamera = compElem.BoolAttribute("MainCamera");
}

void Camera::SaveXML(Bulb::XMLElement compElem)
{
	compElem.SetAttribute("ComponentType", "Camera");
	compElem.SetAttribute("MainCamera", _isMainCamera);
}

ComponentSnapshot Camera::CaptureSnapshot()
{
	ComponentSnapshot snapshot;

	snapshot.id = _id;
	snapshot.componentType = "Camera";

	snapshot.datas.push_back(IsMainCamera() ? 1 : 0);

	return snapshot;
}

void Camera::RestoreSnapshot(ComponentSnapshot snapshot)
{
	if (snapshot.datas[0] == 1)
		SetAsMainCamera();
}

XMFLOAT3& Camera::GetEyePos()
{
#ifdef BULB_EDITOR
	if (EDITOR->IsOnPlay())
		return _currentCamera->GetTransform()->GetPosition();
	else
		return _editorCamera->GetTransform()->GetPosition();
#endif
	return _currentCamera->GetTransform()->GetPosition();
}

XMFLOAT4X4& Camera::GetViewMatrix()
{
#ifdef BULB_EDITOR
	if (EDITOR->IsOnPlay())
		return _currentCamera->_matView;
	else
		return _editorCamera->_matView;
#endif
	return _currentCamera->_matView;
}

XMFLOAT4X4& Camera::GetProjMatrix()
{
#ifdef BULB_EDITOR
	if (EDITOR->IsOnPlay())
		return _currentCamera->_matProj;
	else
		return _editorCamera->_matProj;
#endif
	return _currentCamera->_matProj;
}

XMFLOAT4X4& Camera::GetViewProjMatrix()
{
#ifdef BULB_EDITOR
	if (EDITOR->IsOnPlay())
		return _currentCamera->_matViewProj;
	else
		return _editorCamera->_matViewProj;
#endif
	return _currentCamera->_matViewProj;
}

XMFLOAT4X4& Camera::GetOrthoMatrix()
{
#ifdef BULB_EDITOR
	if (EDITOR->IsOnPlay())
		return _currentCamera->_matOrtho;
	else
		return _editorCamera->_matOrtho;
#endif
	return _currentCamera->_matOrtho;
}

int Camera::GetFramesDirty()
{
#ifdef BULB_EDITOR
	if (EDITOR->IsOnPlay())
		return _currentCamera->GetGameObject()->GetFramesDirty();
	else
		return _editorCamera->GetGameObject()->GetFramesDirty();
#endif
	return _currentCamera->GetGameObject()->GetFramesDirty();
}

void Camera::SetAsMainCamera()
{
	if (_currentCamera != nullptr) _currentCamera->_isMainCamera = false;
	_isMainCamera = true;
	_currentCamera = static_pointer_cast<Camera>(shared_from_this());
}
