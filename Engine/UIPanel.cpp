#include "pch.h"
#include "UIPanel.h"

UIPanel::UIPanel() : UIElement(UIType::Panel)
{

}

UIPanel::~UIPanel()
{

}

void UIPanel::Render(ID3D12GraphicsCommandList* cmdList)
{

}

void UIPanel::SetTexture(shared_ptr<Texture> texture)
{
	if (texture == nullptr) {
		SetTexture(L"Tex_Default");
		return;
	}

	_textureSrvHeapIndex = texture->GetSRVHeapIndex();
	_textureName = texture->GetName();
}

void UIPanel::SetTexture(wstring textureName)
{
	// 텍스쳐 이름만 설정하고, 실제 텍스쳐는 기본 텍스쳐로
	_textureSrvHeapIndex = 0;
	_textureName = Utils::ToString(textureName);
}
