#pragma once

#define DEFAULT_UI_COUNT 10

class UIManager
{
	DECLARE_SINGLE(UIManager)
public:
	~UIManager();

	void Init();
	void Render(ID3D12GraphicsCommandList* cmdList);

public:
	UINT GetUIBufferSRVIndex() { return _uploadBufferSrvIndex; }

	void CreateBuffer();
	void AddUI(const shared_ptr<UIElement>& ui);

	void CreateTestUI();

private:
	int _uiCount = 0;

	shared_ptr<Mesh> _quadMesh;

	vector<shared_ptr<UIElement>> _uiArray;
	vector<UIInstanceConstants> _uiConstants;

	unique_ptr<UploadBuffer<UIInstanceConstants>> _uploadBuffer = nullptr;
	UINT _uploadBufferSrvIndex = -1;
};
