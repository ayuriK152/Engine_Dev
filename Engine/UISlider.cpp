#include "pch.h"
#include "UISlider.h"

UISlider::UISlider() : UIElement(UIType::Slider)
{
	_name = "UISlider";

	_size = { 200.0f, 20.0f };

	background = UI->CreateUI<UIPanel>();
	fill = UI->CreateUI<UIPanel>();
}

UISlider::~UISlider()
{

}

void UISlider::Init()
{
	_transform->SetPivot({ 0.0f, 1.0f });

	background->GetTransform()->SetParent(_transform);
	background->SetColor({ 0.7f, 0.7f, 0.7f, 1.0f });
	background->SetSize(_size);
	background->GetTransform()->SetDepth(_transform->GetDepth() + 1.0f);

	fill->GetTransform()->SetParent(_transform);
	fill->GetTransform()->SetPivot({ 0.0f, 0.5f });
	fill->GetTransform()->SetLocalPosition({ -100.0f, 0.0f, 0.0f });
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
