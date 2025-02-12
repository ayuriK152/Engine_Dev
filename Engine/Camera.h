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

	static XMFLOAT4X4& GetViewMatrix() { return _mainMatView; }
	static XMFLOAT4X4& GetProjMatrix() { return _mainMatProj; }
	
private:
	static XMFLOAT4X4 _mainMatView;
	static XMFLOAT4X4 _mainMatProj;

	XMFLOAT4X4 _matView;
	XMFLOAT4X4 _matProj;
	float _aspectRatio;
};

