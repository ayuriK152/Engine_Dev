#include "pch.h"
#include "UITransform.h"

UITransform::UITransform(shared_ptr<UIElement> element)
{
	_element = element;
}

UITransform::~UITransform()
{
	cout << "Released - UITransform\n";
}

void UITransform::UpdateTransform()
{
	if (!_isDirty) return;

	if (_parent != nullptr) {
		_position = _parent->GetPosition() + _localPosition;
	}
	else {
		_position = _localPosition;
	}

	_isDirty = false;
	for (auto& child : _childs) {
		child->SetDirtyFlag();
	}
}

void UITransform::SetPivot(const Vector2& pivot)
{
	_pivot = pivot;

	Vector2 size = _element.lock()->GetSize();
	float width = size.x * (_pivot.x - 0.5f);
	float height = size.y * (0.5f - _pivot.y);

	_localPosition.x = _localPosition.x + width;
	_localPosition.y = _localPosition.y + height;
}

void UITransform::SetPosition(const Vector3& position)
{
	_position = position;
	if (_parent != nullptr) {
		_localPosition = _position - _parent->GetPosition();
	}
	else {
		_localPosition = _position;
	}

	SetDirtyFlag();
}

void UITransform::SetLocalPosition(const Vector3& position)
{
	_localPosition = position;

	if (_parent != nullptr) {
		_position = _parent->GetPosition() + _localPosition;
	}
	else {
		_position = _localPosition;
	}

	SetDirtyFlag();
}

void UITransform::SetParent(shared_ptr<UITransform> parent)
{
	_parent = parent;

	if (_parent != nullptr) {
		UpdateTransform();
		_parent->AddChild(shared_from_this());
	}
}

void UITransform::AddChild(shared_ptr<UITransform> child)
{
	_childs.push_back(child);
}

void UITransform::SetDirtyFlag()
{
	_isDirty = true;
	for (auto& child : _childs) {
		child->SetDirtyFlag();
	}
}
