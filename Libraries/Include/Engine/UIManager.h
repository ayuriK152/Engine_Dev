#pragma once

#define DEFAULT_UI_COUNT 10

class UIManager
{
	DECLARE_SINGLE(UIManager)
public:
	~UIManager();

	void Init();
	void Update();
	void Render(ID3D12GraphicsCommandList* cmdList);

public:
	template<typename T>
	shared_ptr<T> CreateUI(string name = "");

	UINT GetUIBufferSRVIndex() { return _uploadBufferSrvIndex; }

	void AddUI(const shared_ptr<UIElement>& ui);
	void DeleteUI(const shared_ptr<UIElement>& ui);

	Vector2 GetUIResolution() { return _uiResolution; }

private:
	void CreateBuffer();

private:
	Vector2 _uiResolution = { 1920.0f, 1080.0f };
	int _elementCount = 0;
	int _panelCount = 0;

	shared_ptr<Mesh> _quadMesh;

	vector<shared_ptr<UIElement>> _elements;
	vector<shared_ptr<UIPanel>> _panels;
	vector<UIInstanceConstants> _uiConstants;

	unique_ptr<UploadBuffer<UIInstanceConstants>> _uploadBuffer = nullptr;
	UINT _uploadBufferSrvIndex = -1;
};

template<typename T>
shared_ptr<T> UIManager::CreateUI(string name)
{
	if (!is_base_of_v<UIElement, T>) {
		DEBUG->ErrorLog("[UIManager] - Incorrect UI class!");
		return nullptr;
	}

	shared_ptr<T> element = make_shared<T>();
	element->_transform = make_shared<UITransform>(element);
	element->Init();
	AddUI(element);

	return element;
}
