#pragma once
#include "UIElement.h"
class UIPanel :  public UIElement
{
public:
	UIPanel();
	~UIPanel() override;

public:
	void Render(ID3D12GraphicsCommandList* cmdList) override;

public:
	void SetTexture(shared_ptr<Texture> texture);
	void SetTexture(wstring textureName);

private:
	string _textureName;
	int _textureSrvHeapIndex;
};

