#pragma once
#include "UIElement.h"
class UIPanel :  public UIElement
{
public:
	UIPanel();
	~UIPanel();

public:
	void Render(ID3D12GraphicsCommandList* cmdList) override;
};

