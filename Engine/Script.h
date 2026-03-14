#pragma once
#include "Component.h"
class Script : public Component
{
	using Super = Component;
public:
	Script();
	virtual ~Script();

	virtual void OnDestroy() override = 0;

	virtual void LoadXML(XMLElement* compElem) override = 0;
};

