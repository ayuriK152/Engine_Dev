#pragma once

class UIElement;

class UITransform : public enable_shared_from_this<UITransform>
{
	friend class UITransform;
public:
	UITransform(shared_ptr<UIElement> element);
	~UITransform();

public:
	void UpdateTransform();

	void SetPivot(const Vector2& pivot);
	void SetPosition(const Vector3& position);
	void SetLocalPosition(const Vector3& position);
	void SetDepth(float depth) { _depth = depth; }
	void SetDynamicPosition(bool value) { _isDynamicPosition = value; }

	Vector2 GetPivot() { return _pivot; }
	Vector3 GetPosition() { 
		if (_isDirty) UpdateTransform();
		return _position; 
	}
	Vector3 GetLocalPosition() {
		if (_isDirty) UpdateTransform();
		return _localPosition; 
	}
	float GetDepth() { 
		return _depth; 
	}
	bool IsDynamicPosition() { return _isDynamicPosition; }

	void SetParent(shared_ptr<UITransform> parent);
	void AddChild(shared_ptr<UITransform> child);

	shared_ptr<UIElement> GetUIElement() { return _element.lock(); }

private:
	void SetDirtyFlag();

private:
	bool _isDirty = true;

	Vector2 _pivot = { 0.5f, 0.5f };

	bool _isDynamicPosition = false;
	Vector3 _position = { 0.0f, 0.0f, 0.0f };
	Vector3 _localPosition = { 0.0f, 0.0f, 0.0f };
	float _depth = 1.0f;

	shared_ptr<UITransform> _parent = nullptr;
	vector<shared_ptr<UITransform>> _childs;

	weak_ptr<UIElement> _element;
};

