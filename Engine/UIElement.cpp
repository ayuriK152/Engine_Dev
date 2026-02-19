#include "pch.h"
#include "UIElement.h"

UIElement::UIElement(UIType type) : _type(type)
{

}

UIElement::~UIElement()
{

}

void UIElement::SetTexture(shared_ptr<Texture> texture)
{
	if (texture == nullptr) {
		SetTexture(L"Tex_Default");
		return;
	}

	_textureSrvHeapIndex = texture->GetSRVHeapIndex();
	_textureName = texture->GetName();
}

void UIElement::SetTexture(wstring textureName)
{
	auto texture = RESOURCE->Get<Texture>(textureName);
	if (texture != nullptr) {
		_textureSrvHeapIndex = texture->GetSRVHeapIndex();
		_textureName = Utils::ToString(textureName);
	}
}
