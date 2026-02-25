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

}
