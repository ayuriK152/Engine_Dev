#pragma once

class Graphic
{
	DECLARE_SINGLE(Graphic);
private:
	Graphic(const Graphic& rhs) = delete;
	Graphic& operator=(const Graphic& rhs) = delete;
	~Graphic();

	friend class GameApplication;

public:
	HWND GetMainWnd()const { return _hMainWnd; }
	ComPtr<IDXGIFactory4> GetDXGIFactory()const { return _dxgiFactory; }

	float GetAspectRatio()const { return static_cast<float>(_appDesc.clientWidth) / _appDesc.clientHeight; }

	bool Get4xMsaaState()const { return _appDesc._4xMsaaState; }
	void Set4xMsaaState(bool value) {
		if (_appDesc._4xMsaaState != value)
		{
			_appDesc._4xMsaaState = value;

			BuildSwapChain();
			OnResize();
		}
	}

	AppDesc GetAppDesc()const { return _appDesc; }
	void SetAppDesc(AppDesc appDesc) { _appDesc = appDesc; }

	int GetNumFrameResources()const { return _numFrameResources; }
	FrameResource* GetCurrFrameResource()const { return _currFrameResource; }
	int GetCurrFrameResourceIndex()const { return _currFrameResourceIndex; }

	ComPtr<ID3D12Device> GetDevice()const { return _device; }
	ComPtr<ID3D12GraphicsCommandList> GetCommandList()const { return _commandList; }
	ComPtr<ID3D12CommandQueue> GetCommandQueue()const { return _commandQueue; }

	ComPtr<ID3D12DescriptorHeap> GetConstantBufferHeap()const { return _cbvHeap; }
	ComPtr<ID3D12DescriptorHeap> GetShaderResourceViewHeap()const { return _srvHeap; }

	ID3D12Resource* GetCurrentBackBuffer()const { return _swapChainBuffer[_currBackBuffer].Get(); }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferView()const {
		return CD3DX12_CPU_DESCRIPTOR_HANDLE(
			_rtvHeap->GetCPUDescriptorHandleForHeapStart(),
			_currBackBuffer,
			_rtvDescriptorSize);
	}
	D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView()const { return _dsvHeap->GetCPUDescriptorHandleForHeapStart(); }

	UINT GetCBVSRVDescriptorSize()const { return _cbvSrvUavDescriptorSize; }

	DXGI_FORMAT GetBackBufferFormat()const { return _backBufferFormat; }

	vector<shared_ptr<GameObject>>& GetObjects() { return _objects; }

public:
	bool Initialize();
	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	shared_ptr<GameObject> AddGameObject(shared_ptr<GameObject> obj);

private:
	void OnResize();
	void Update();
	void UpdateCamera();
	void UpdateMainCB();
	void Render();
	void RenderBegin();
	void RenderEnd();

private:
	bool InitMainWindow();
	bool InitDirect3D();

	void BuildCommandObjects();
	void BuildSwapChain();
	void BuildRtvAndDsvDescriptorHeaps();

	void BuildDescriptorHeaps();
	void BuildRootSignature();
	void BuildShaderAndInputLayout();
	void BuildFrameResources();

	void FlushCommandQueue();

	//=========================

	array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();

private:
	HWND      _hMainWnd = nullptr;

	ComPtr<IDXGIFactory4> _dxgiFactory;
	ComPtr<IDXGISwapChain> _swapChain;
	ComPtr<ID3D12Device> _device;

	ComPtr<ID3D12Fence> _fence;
	UINT64 _currentFence = 0;

	ComPtr<ID3D12CommandQueue> _commandQueue;
	ComPtr<ID3D12CommandAllocator> _directCmdListAlloc;
	ComPtr<ID3D12GraphicsCommandList> _commandList;

	static const int _SwapChainBufferCount = 2;
	int _currBackBuffer = 0;
	ComPtr<ID3D12Resource> _swapChainBuffer[_SwapChainBufferCount];
	ComPtr<ID3D12Resource> _depthStencilBuffer;

	ComPtr<ID3D12RootSignature> _rootSignature;

	ComPtr<ID3D12DescriptorHeap> _rtvHeap;
	ComPtr<ID3D12DescriptorHeap> _dsvHeap;
	ComPtr<ID3D12DescriptorHeap> _cbvHeap;
	ComPtr<ID3D12DescriptorHeap> _srvHeap;

	D3D12_VIEWPORT _screenViewport;
	D3D12_RECT _scissorRect;

	UINT _rtvDescriptorSize = 0;
	UINT _dsvDescriptorSize = 0;
	UINT _cbvSrvUavDescriptorSize = 0;

	D3D_DRIVER_TYPE _d3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
	DXGI_FORMAT _backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT _depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	vector<D3D12_INPUT_ELEMENT_DESC> _inputLayout;

	unordered_map<string, ComPtr<ID3D12PipelineState>> _PSOs;

	vector<shared_ptr<GameObject>> _objects;

	static const int _numFrameResources = 3;
	int _currFrameResourceIndex = 0;
	FrameResource* _currFrameResource = nullptr;
	vector<unique_ptr<FrameResource>> _frameResources;

	AppDesc _appDesc;

	//===========================리팩토링 필수!!!!!!!!============

	// 얘네는 카메라로 분할==========
	XMFLOAT4X4 _world = MathHelper::Identity4x4();
	XMFLOAT4X4 _view = MathHelper::Identity4x4();
	XMFLOAT4X4 _proj = MathHelper::Identity4x4();

	Vector3 _cameraPos = { 0.0f, 3.0f, -5.0f };

	Vector3 _eyePos;
	// ============================

	UINT _passCbvOffset = 0;

	PassConstants _mainPassCB;

	UINT _objCBByteSize = DXUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
};

