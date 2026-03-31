#pragma once
#include "Component.h"

class Camera : public Component
{
	friend class Camera;
	using Super = Component;
public:
	Camera();
	virtual ~Camera();

	void Init() override;
	void Update() override;

	void OnDestroy() override;

	void LoadXML(XMLElement* compElem) override;
	void SaveXML(XMLElement* compElem) override;

	ComponentSnapshot CaptureSnapshot() override;
	void RestoreSnapshot(ComponentSnapshot snapshot) override;

public:
	static shared_ptr<Camera> GetCurrentCamera() { return _currentCamera; }

	static XMFLOAT3& GetEyePos();

	static XMFLOAT4X4& GetViewMatrix();

	static XMFLOAT4X4& GetProjMatrix();

	static XMFLOAT4X4& GetViewProjMatrix();

	static XMFLOAT4X4& GetOrthoMatrix();

	static int GetFramesDirty();
	
	void SetAsMainCamera();
	bool IsMainCamera() { return _isMainCamera; }

private:
	XMFLOAT4X4 _matView;
	XMFLOAT4X4 _matProj;
	XMFLOAT4X4 _matViewProj;
	XMFLOAT4X4 _matOrtho;
	float _aspectRatio;
	Bulb::Vector2 _viewportSize;

	bool _isMainCamera = false;
	static shared_ptr<Camera> _currentCamera;
	static UINT _cameraCount;

	static shared_ptr<Camera> _editorCamera;
};

