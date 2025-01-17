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
	HWND GetMainWnd()const;

	float GetAspectRatio()const;

	bool Get4xMsaaState()const;
	void Set4xMsaaState(bool value);

	AppDesc GetAppDesc()const;
	void SetAppDesc(AppDesc appDesc);

	int GetNumFrameResources()const;

	ComPtr<ID3D12Device> GetDevice()const;
	ComPtr<ID3D12GraphicsCommandList> GetCommandList()const;
	ComPtr<ID3D12DescriptorHeap> GetConstantBufferHeap()const;

	bool Initialize();
	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	void CreateRtvAndDsvDescriptorHeaps();
	void OnResize();
	void Update();
	void Render();
	void RenderBegin();
	void RenderEnd();

private:
	bool InitMainWindow();
	bool InitDirect3D();

	void BuildCommandObjects();
	void BuildSwapChain();

	void BuildDescriptorHeaps();
	void BuildConstantBuffers();
	void BuildRootSignature();
	void BuildShaderAndInputLayout();
	void BuildObjectGeometry();
	void BuildPSO();

	void FlushCommandQueue();

	ID3D12Resource* CurrentBackBuffer()const;
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView()const;
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView()const;

	void CalculateFrameStats();

	void LogAdapters();
	void LogAdapterOutputs(IDXGIAdapter* adapter);
	void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);

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

	D3D12_VIEWPORT _screenViewport;
	D3D12_RECT _scissorRect;

	UINT _rtvDescriptorSize = 0;
	UINT _dsvDescriptorSize = 0;
	UINT _cbvSrvUavDescriptorSize = 0;

	D3D_DRIVER_TYPE _d3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
	DXGI_FORMAT _backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT _depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	unordered_map<string, ComPtr<ID3DBlob>> _shaders;
	vector<D3D12_INPUT_ELEMENT_DESC> _inputLayout;

	unordered_map<string, ComPtr<ID3D12PipelineState>> _PSOs;
	unordered_map<string, unique_ptr<Geometry>> _geometrys;

	vector<unique_ptr<GameObject>> _objects;

	static const int _numFrameResources = 3;

	AppDesc _appDesc;

	//===========================�����丵 �ʼ�!!!!!!!!============

	unique_ptr<UploadBuffer<ObjectConstants>> _objectCB = nullptr;

	XMFLOAT4X4 _world = MathHelper::Identity4x4();
	XMFLOAT4X4 _view = MathHelper::Identity4x4();
	XMFLOAT4X4 _proj = MathHelper::Identity4x4();
};

