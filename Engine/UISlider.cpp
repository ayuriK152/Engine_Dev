#include "pch.h"
#include "UISlider.h"

UISlider::UISlider() : UIElement(UIType::Slider)
{
	_name = "UISlider";

	SetPivot({ 0.0f, 1.0f });
	_size = { 200.0f, 20.0f };

	background = UI->CreateUI<UIPanel>();
	fill = UI->CreateUI<UIPanel>();
}

UISlider::~UISlider()
{

}

void UISlider::Init()
{
	background->SetParent(shared_from_this());
	background->SetColor({ 0.7f, 0.7f, 0.7f, 1.0f });
	background->SetSize(_size);
	background->SetDepth(_depth + 1.0f);

	fill->SetParent(shared_from_this());
	fill->SetPivot({ 0.0f, 0.5f });
	fill->SetLocalPosition({ -100.0f, 0.0f, 0.0f });
	fill->SetSize(_size);
}

void UISlider::Update()
{
	if (_isDirty) {
		if (_min > _max) _max = _min;
		if (_max < _min) _min = _max;
		_value = clamp(_value, _min, _max);

		fill->SetSize({ _size.x * _value / (_max - _min), _size.y });

		_isDirty = false;
	}
}

void UISlider::SetLocalPosition(const Vector3 position)
{
	_localPosition = position;
	background->SetLocalPosition(position);
	fill->SetLocalPosition({ position.x - 100.0f, position.y, position.z });
}
