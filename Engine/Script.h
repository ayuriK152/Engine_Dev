#pragma once
#include "Component.h"
class Script : public Component
{
	using Super = Component;
public:
	Script();
	virtual ~Script();

	virtual void Init() override = 0;
	virtual void FixedUpdate() override = 0;
	virtual void Update() override = 0;
	virtual void Render() override = 0;
};

