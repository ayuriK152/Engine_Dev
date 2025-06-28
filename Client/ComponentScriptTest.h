#pragma once
#include "Script.h"
class ComponentScriptTest : public Script
{
	void Init() override;
	void FixedUpdate() override;
	void Update() override;
	void Render() override;
};

