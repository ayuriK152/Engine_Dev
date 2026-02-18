#pragma once

class UIElement : public enable_shared_from_this<UIElement>
{
	friend class UIManager;
public:
	UIElement(UIType type);
	virtual ~UIElement() = 0;

public:
	virtual void Init() { }
	virtual void Update() { }
	virtual void Render(ID3D12GraphicsCommandList* cmdList) { }

protected:
	UIType _type;

	Vector2 _pivot = { 0.5f, 0.5f };

	Vector3 _localPosition = { 0.0f, 0.0f, 0.0f };
	Vector2 _size = { 100.0f, 100.0f };

	XMFLOAT4X4 _mat;
};
