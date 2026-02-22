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
	void SetPivot(const Vector2 pivot);
	void SetLocalPosition(const Vector3 position);
	void SetSize(const Vector2 size);
	void SetDepth(float depth) { _depth = depth; }
	void SetDynamicPosition(bool value) { _isDynamicPosition = value; }

	string GetName() { return _name; }
	Vector2 GetPivot() { return _pivot; }
	Vector3 GetLocalPosition() { return _localPosition; }
	Vector2 GetSize() { return _size; }
	float GetDepth() { return _depth; }
	bool IsDynamicPosition() { return _isDynamicPosition; }

	void SetParent(shared_ptr<UIElement> parent);
	void AddChild(shared_ptr<UIElement> child);

protected:
	string _name;
	UIType _type;

	Vector2 _pivot = { 0.5f, 0.5f };

	bool _isDynamicPosition = false;
	Vector3 _postion = { 0.0f, 0.0f, 0.0f };
	Vector3 _localPosition = { 0.0f, 0.0f, 0.0f };
	Vector2 _size = { 100.0f, 100.0f };
	float _depth = 1.0f;

	shared_ptr<UIElement> _parent = nullptr;
	vector<shared_ptr<UIElement>> _childs;
};
