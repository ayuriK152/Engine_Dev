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

	float width = _size.x * (_pivot.x - 0.5f);
	float height = _size.y * (_pivot.y - 0.5f);

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

Vector3 UITransform::GetPosition()
{
	if (_isDirty) UpdateTransform();
	float width = _size.x * (0.5f - _pivot.x);
	float height = _size.y * (0.5f - _pivot.y);
	return { _position.x + width, _position.y + height, _position.z };
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
