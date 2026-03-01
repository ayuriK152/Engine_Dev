#include "pch.h"
#include "UIElement.h"

UIElement::UIElement(UIType type) : _type((UINT)type)
{
	_name = "UIElement";
}

UIElement::UIElement(UINT type) : _type(type)
{
	_name = "UIElement";
}

UIElement::~UIElement()
{
	cout << "Released - UIElement\n";
}

void UIElement::SetRenderActive(bool value)
{
	int deltaCount = value ? -1 : 1;
	_renderActive += deltaCount;

	vector<shared_ptr<UITransform>> childs = _transform->GetChilds();
	for (auto& c : childs) {
		c->GetUIElement()->SetRenderActive(value);
	}
}
