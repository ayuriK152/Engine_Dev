#pragma once
#include "Component.h"

class Camera : public Component
{
	using Super = Component;
public:
	Camera();
	virtual ~Camera();

	void Init() override;
	void Update() override;

public:
	static XMFLOAT3& GetEyePos() { return _currentCamera->_eyePos; }
	static XMFLOAT4X4& GetViewMatrix() { return _currentCamera->_matView; }
	static XMFLOAT4X4& GetProjMatrix() { return _currentCamera->_matProj; }
	
	void SetAsMainCamera() { _currentCamera = this; }
	bool IsMainCamera() { return _currentCamera == this; }

private:
	XMFLOAT3 _eyePos;
	XMFLOAT4X4 _matView;
	XMFLOAT4X4 _matProj;
	float _aspectRatio;

	static Camera* _currentCamera;
	static UINT _cameraCount;
};

