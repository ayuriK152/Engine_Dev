#pragma once
#include "UIElement.h"
class UIPanel :  public UIElement
{
	friend class UIManager;
public:
	UIPanel();
	UIPanel(UIType type);
	~UIPanel();

public:
	void Render(ID3D12GraphicsCommandList* cmdList) override;

public:
	void SetTexture(shared_ptr<Texture> texture);
	void SetTexture(wstring textureName);

	void SetColor(const ColorRGBA color) { _color = color; }
	ColorRGBA GetColor() { return _color; }

protected:
	ColorRGBA _color = { 1.0f, 1.0f, 1.0f, 1.0f };

	string _textureName;
	int _textureSrvHeapIndex;
};

