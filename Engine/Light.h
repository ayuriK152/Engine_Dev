#pragma once
#include "Component.h"

class Light : public Component
{
	using Super = Component;
public:
	Light();
	Light(XMFLOAT4 ambient, XMFLOAT4 diffuse, XMFLOAT4 specular);
	virtual ~Light();

	virtual void Init() = 0;
	virtual void Update() = 0;

public:
	void SetFramesDirty();
	int GetFramesDirty() { return _numFramesDirty; }
	void ReleaseFramesDirty() { _numFramesDirty -= 1; }

	virtual LightConstants GetLightConstants() = 0;

public:
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular;

private:
	int _numFramesDirty = 0;
};

