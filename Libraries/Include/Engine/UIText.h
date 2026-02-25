#pragma once
#include "UIElement.h"
class UIText : public UIPanel
{
public:
	UIText();
	~UIText();

	void Init() override;
	void Update() override;

public:
	void SetText(const wstring& text);
	void SetFont(const wstring& fontName);
	void SetFontSize(float fontSize);
	void SetTextAlignment(DWRITE_TEXT_ALIGNMENT textAlignment) { 
		_textAlignment = textAlignment;
		_textFormat->SetTextAlignment(_textAlignment);
	}
	void SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT paragraphAlignment) {
		_paragraphAlignment = paragraphAlignment;
		_textFormat->SetParagraphAlignment(_paragraphAlignment);
	}
	void SetTextColor(const ColorRGBA& color);

	void SetSize(const Vector2 size) { 
		_size = size; 
		_isDirty = true;
	}

private:
	void BuildDescriptors();
	void BuildResource();
	void RenderTextToTexture();

	void UpdateTextFormat();
	void UpdateTextLayout();
	void UpdateBrush();

private:
	wstring _text = L"Default Text";
	wstring _fontName = L"Malgun Gothic";
	float _fontSize = 24.0f;
	ColorRGBA _textColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	DWRITE_TEXT_ALIGNMENT _textAlignment = DWRITE_TEXT_ALIGNMENT_CENTER;
	DWRITE_PARAGRAPH_ALIGNMENT _paragraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;

	DWRITE_TEXT_METRICS _metrics;

	ComPtr<IDWriteTextLayout>	_textLayout;
	ComPtr<IDWriteTextFormat>	_textFormat;
	ComPtr<ID2D1SolidColorBrush> _brush;

	ComPtr<ID3D12Resource>		_textTexture;
	ComPtr<ID3D11Resource>		_wrappedResource;
	ComPtr<ID2D1Bitmap1>		_d2dBitmap;

	D3D12_GPU_DESCRIPTOR_HANDLE _srvHandle;

	bool _isDirty = true;
};

