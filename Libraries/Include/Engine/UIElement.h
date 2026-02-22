#pragma once

class UIElement : public enable_shared_from_this<UIElement>
{
	friend class UIManager;
protected:
	UIElement(UIType type);
	virtual ~UIElement();

public:
	virtual void Init() { }
	virtual void Update() { }
	virtual void Render(ID3D12GraphicsCommandList* cmdList) { }

public:
	void SetPivot(const Vector2 pivot) { _pivot = pivot; }
	void SetLocalPosition(const Vector3 position) { _localPosition = position; }
	void SetSize(const Vector2 size) { _size = size; }

	Vector2 GetPivot() { return _pivot; }
	Vector3 GetLocalPosition() { return _localPosition; }
	Vector2 GetSize() { return _size; }

protected:
	string _name;
	UIType _type;

	Vector2 _pivot = { 0.5f, 0.5f };

	Vector3 _localPosition = { 0.0f, 0.0f, 0.0f };
	Vector2 _size = { 100.0f, 100.0f };
};
