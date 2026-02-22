#pragma once
#include "UIElement.h"
class UISlider : public UIElement
{
	friend class UIManager;
public:
	UISlider();
	~UISlider();

	void Init() override;
	void Update() override;

public:
	shared_ptr<UIPanel> background;
	shared_ptr<UIPanel> fill;

	void SetLocalPosition(const Vector3 position);

	void SetValue(float value) {
		_value = value;
		_isDirty = true;
	}

	void SetValueMinLimit(float value) {
		_min = value;
		_isDirty = true;
	}

	void SetValueMaxLimit(float value) {
		_max = value;
		_isDirty = true;
	}

	void SetBackgroundColor(const ColorRGBA color) { background->SetColor(color); }
	void SetFillColor(const ColorRGBA color) { fill->SetColor(color); }

private:
	bool _isDirty = true;

	float _value = 5.0f;
	float _min = 0.0f;
	float _max = 10.0f;
};

