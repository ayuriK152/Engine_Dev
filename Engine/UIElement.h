#pragma once

class UIElement : public enable_shared_from_this<UIElement>
{
	friend class UIManager;
protected:
	UIElement(UIType type);
	UIElement(UINT type);
	virtual ~UIElement();

public:
	virtual void Init() { }
	virtual void Update() { }
	virtual void Render(ID3D12GraphicsCommandList* cmdList) { }

public:
	void SetName(const string name) { _name = name; }
	string GetName() { return _name; }

	shared_ptr<UITransform> GetTransform() { return _transform; }

	// Counter method, Avoid duplicate use
	void SetRenderActive(bool value);
	bool IsRenderActive() { return _renderActive == 0; }

protected:
	string _name;
	UINT _type;

	shared_ptr<UITransform> _transform;

	UINT _renderActive = 0;
};
