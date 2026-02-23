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
	void SetName(const string name) { _name = name; }
	string GetName() { return _name; }

	void SetSize(const Vector2 size) { _size = size; }
	Vector2 GetSize() { return _size; }

	shared_ptr<UITransform> GetTransform() { return _transform; }

protected:
	string _name;
	UIType _type;

	Vector2 _size = { 100.0f, 100.0f };

	shared_ptr<UITransform> _transform;
};
