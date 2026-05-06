#pragma once
#include "UIElement.h"

class BULB_API UIButton : public UIElement
{
	friend class UIManager;
public:
	UIButton();
	~UIButton();

	void Init() override;
	void Update() override;

	void LoadXML(XMLElement* uiElem) override;

	void OnMouseEnter() override;
	void OnMouseExit() override;
	void OnMouseDown() override;

public:
	Event<> buttonEvent;

	shared_ptr<UIPanel> background;
	shared_ptr<UIText> text;

	Bulb::Color colorDefault = { 1.0f, 1.0f, 1.0f, 1.0f };
	Bulb::Color colorHovered = { 0.7f, 0.7f, 0.7f, 1.0f };
	Bulb::Color colorClicked = { 0.55f, 0.55f, 0.55f, 1.0f };
};

