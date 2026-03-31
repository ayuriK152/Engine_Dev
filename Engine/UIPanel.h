#pragma once
#include "UIElement.h"

class UIPanel :  public UIElement
{
	friend class UIManager;
public:
	UIPanel();
	UIPanel(UIType type);
	virtual ~UIPanel();

public:
	void Render(ID3D12GraphicsCommandList* cmdList) override;

public:
	void SetTexture(shared_ptr<Texture> texture);
	void SetTexture(wstring textureName);

	void SetColor(const Bulb::Color color) { _color = color; }
	Bulb::Color GetColor() { return _color; }

protected:
	Bulb::Color _color = { 1.0f, 1.0f, 1.0f, 1.0f };

	string _textureName;
	int _textureSrvHeapIndex;
};

