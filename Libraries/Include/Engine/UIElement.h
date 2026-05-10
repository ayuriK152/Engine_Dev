#pragma once

class BULB_API UIElement : public enable_shared_from_this<UIElement>
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

	virtual void OnDestroy() { };

	virtual void OnMouseEnter() { _isHovered = true; }
	virtual void OnMouseExit() { _isHovered = false; _isClicked = false; }
	virtual void OnMouseDown() { if (_isHovered) _isClicked = true; }
	virtual void OnMouseUp() { _isClicked = false; }

	virtual void LoadXML(XMLElement* uiElem) = 0;

public:
	void SetName(const string name) { _name = name; }
	string GetName() { return _name; }

	shared_ptr<UITransform> GetTransform() { return _transform; }

	// Counter method, Avoid duplicate use
	void SetRenderActive(bool value);
	bool IsRenderActive() { return _renderActive == 0; }

	void SetPassthroughMouse(bool value) { _passthroughMouse = value; }

protected:
	string _name;
	UINT _type;

	shared_ptr<UITransform> _transform;

	UINT _renderActive = 0;

	// Mouse Input
	bool _isHovered;
	bool _isClicked;

	bool _passthroughMouse = false;
};
