#pragma once
#include "UIElement.h"

class BULB_API UIFrame : public UIElement
{
public:
	UIFrame();
	~UIFrame();

	void LoadXML(XMLElement* uiElem) override;
};

