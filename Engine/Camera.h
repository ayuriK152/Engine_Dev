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

};

