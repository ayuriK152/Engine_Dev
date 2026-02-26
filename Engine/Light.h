#pragma once
#include "Component.h"

class Light : public Component
{
	using Super = Component;
public:
	Light();
	Light(XMFLOAT4 ambient, XMFLOAT4 diffuse, XMFLOAT4 specular);
	virtual ~Light();

	virtual void Init() override = 0;
	virtual void Update() override = 0;

	virtual void OnDestroy() override = 0;

public:
	void SetFramesDirty();
	int GetFramesDirty() { return _numFramesDirty; }
	void ReleaseFramesDirty() { _numFramesDirty -= 1; }

	XMFLOAT4X4& GetViewMatrix() { return _matView; }
	XMFLOAT4X4& GetProjMatrix() { return _matProj; }

	virtual LightConstants GetLightConstants() = 0;

public:
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular;

protected:
	XMFLOAT4X4 _matView;
	XMFLOAT4X4 _matProj;

private:
	int _numFramesDirty = 0;
};

