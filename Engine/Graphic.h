#pragma once

class Graphic
{
	friend class GameApplication;
	DECLARE_SINGLE(Graphic);
private:
	Graphic(const Graphic& rhs) = delete;
	Graphic& operator=(const Graphic& rhs) = delete;
	~Graphic();

public:
	HWND GetMainWnd() const { return _hMainWnd; }
	ComPtr<IDXGIFactory4> GetDXGIFactory()const { return _dxgiFactory; }

	float GetAspectRatio() const { return static_cast<float>(_appDesc.clientWidth) / _appDesc.clientHeight; }

	bool Get4xMsaaState() const { return _appDesc._4xMsaaState; }
	void Set4xMsaaState(bool value) {
		if (_appDesc._4xMsaaState != value)
		{
			_appDesc._4xMsaaState = value;

			BuildSwapChain();
			OnResize();
		}
	}

	AppDesc GetAppDesc() const { return _appDesc; }
	void SetAppDesc(AppDesc appDesc) { _appDesc = appDesc; }

	ID3D12Device* GetDevice() const { return _device.Get(); }
	ID3D12GraphicsCommandList* GetCommandList() const { return _commandList.Get(); }
	ID3D12CommandQueue* GetCommandQueue() const { return _commandQueue.Get(); }
	IDXGISwapChain* GetSwapChain() const { return _swapChain.Get(); }

	ComPtr<ID3D12DescriptorHeap> GetConstantBufferHeap()const { return _cbvHeap; }

	ID3D12Resource* GetCurrentBackBuffer()const { return _swapChainBuffer[_currBackBuffer].Get(); }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferView()const {
		return CD3DX12_CPU_DESCRIPTOR_HANDLE(
			_rtvHeap->GetCPUDescriptorHandleForHeapStart(),
			_currBackBuffer,
			_rtvDescriptorSize);
	}

	UINT GetAndIncreaseDSVHeapIndex() {
		return _dsvHeapIndex++;
	}
	ComPtr<ID3D12DescriptorHeap> GetDSVHeap()const { return _dsvHeap; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetDSVHandle()const { return _dsvHeap->GetCPUDescriptorHandleForHeapStart(); }

	D3D12_VIEWPORT GetViewport()const { return _screenViewport; }
	D3D12_RECT GetScissorRect()const { return _scissorRect; }
	Vector2 GetScreenCenter()const { return _center; }

	UINT GetDSVDescriptorSize()const { return _dsvDescriptorSize; }
	UINT GetCBVSRVDescriptorSize()const { return _cbvSrvUavDescriptorSize; }

	DXGI_FORMAT GetBackBufferFormat()const { return _backBufferFormat; }
	DXGI_FORMAT GetDepthStencilFormat()const { return _depthStencilFormat; }

public:
	bool Init();
	void RenderEnd(FrameResource* currentFrameResource);

	void WaitForFence(UINT64 fenceValue);
	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	void OnResize();
	void Update();

private:
	bool InitMainWindow();
	bool InitDirect3D();

	void BuildCommandObjects();
	void BuildSwapChain();
	void BuildDescriptorHeaps();

	void FlushCommandQueue();

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

	UINT _dsvHeapIndex = 0;

	ComPtr<ID3D12DescriptorHeap> _rtvHeap;
	ComPtr<ID3D12DescriptorHeap> _dsvHeap;
	ComPtr<ID3D12DescriptorHeap> _cbvHeap;

	D3D12_VIEWPORT _screenViewport;
	D3D12_RECT _scissorRect;
	Vector2 _center;

	UINT _rtvDescriptorSize = 0;
	UINT _dsvDescriptorSize = 0;
	UINT _cbvSrvUavDescriptorSize = 0;

	D3D_DRIVER_TYPE _d3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
	DXGI_FORMAT _backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT _depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	AppDesc _appDesc;

	//===========================리팩토링 필수!!!!!!!!============

	UINT _passCbvOffset = 0;

	UINT _objCBByteSize = DXUtil::CalcConstantBufferByteSize(sizeof(InstanceConstants));
};

