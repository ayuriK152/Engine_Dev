#include "pch.h"
#include "UIElement.h"

UIElement::UIElement(UIType type) : _type(type)
{
	_name = "UIElement";
}

UIElement::~UIElement()
{

}

void UIElement::SetPivot(const Vector2 pivot)
{
	_pivot = pivot;

	float width = _size.x * (_pivot.x - 0.5f);
	float height = _size.y * (0.5f - _pivot.y);

	_localPosition.x = _localPosition.x + width;
	_localPosition.y = _localPosition.y + height;
}

void UIElement::SetLocalPosition(const Vector3 position)
{
	_localPosition = position;
}

void UIElement::SetSize(const Vector2 size)
{
	_size = size;
}

void UIElement::SetParent(shared_ptr<UIElement> parent)
{
	_parent = parent;

	if (_parent != nullptr) {

	}

	_parent->AddChild(shared_from_this());
}

void UIElement::AddChild(shared_ptr<UIElement> child)
{
	_childs.push_back(child);
}
