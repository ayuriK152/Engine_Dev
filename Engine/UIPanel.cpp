#include "pch.h"
#include "UIPanel.h"

UIPanel::UIPanel() : UIElement(UIType::Panel)
{
	_name = "UIPanel";
	SetTexture(L"Tex_Default");
}

UIPanel::UIPanel(UIType type) : UIElement((UINT)UIType::Panel | (UINT)type)
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
	auto texture = RESOURCE->Get<Texture>(textureName);
	if (texture != nullptr) {
		_textureSrvHeapIndex = texture->GetSRVHeapIndex();
		_textureName = Utils::ToString(textureName);
	}
}
