#include "pch.h"
#include "Graphic.h"
#include "Camera.h"

LRESULT CALLBACK
MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return GRAPHIC->MsgProc(hwnd, msg, wParam, lParam);
}

Graphic::~Graphic()
{
	if (_device != nullptr)
		FlushCommandQueue();
}


bool Graphic::Initialize()
{
	if (!InitMainWindow())
		return false;

	if (!InitDirect3D())
		return false;

	OnResize();

	ThrowIfFailed(_commandList->Reset(_directCmdListAlloc.Get(), nullptr));

	RENDER->Init();
	FILEIO->Init();
	if (_appDesc.app != nullptr)
		_appDesc.app->Init();
	ENGINEGUI->Init();
	DEBUG->Init();

	BuildFrameResources();

	ThrowIfFailed(_commandList->Close());
	ID3D12CommandList* cmdsLists[] = { _commandList.Get() };
	_commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	FlushCommandQueue();

	return true;
}


void Graphic::OnResize()
{
	assert(_device);
	assert(_swapChain);
	assert(_directCmdListAlloc);

	FlushCommandQueue();

	ThrowIfFailed(_commandList->Reset(_directCmdListAlloc.Get(), nullptr));

	for (int i = 0; i < _SwapChainBufferCount; ++i)
		_swapChainBuffer[i].Reset();
	_depthStencilBuffer.Reset();

	ThrowIfFailed(_swapChain->ResizeBuffers(
		_SwapChainBufferCount,
		_appDesc.clientWidth, _appDesc.clientHeight,
		_backBufferFormat,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

	_currBackBuffer = 0;

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(_rtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < _SwapChainBufferCount; i++)
	{
		ThrowIfFailed(_swapChain->GetBuffer(i, IID_PPV_ARGS(&_swapChainBuffer[i])));
		_device->CreateRenderTargetView(_swapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
		rtvHeapHandle.Offset(1, _rtvDescriptorSize);
	}

	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = _appDesc.clientWidth;
	depthStencilDesc.Height = _appDesc.clientHeight;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;

	depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;

	depthStencilDesc.SampleDesc.Count = _appDesc._4xMsaaState ? 4 : 1;
	depthStencilDesc.SampleDesc.Quality = _appDesc._4xMsaaState ? (_appDesc._4xMsaaQuality - 1) : 0;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE optClear;
	optClear.Format = _depthStencilFormat;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;
	ThrowIfFailed(_device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthStencilDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&optClear,
		IID_PPV_ARGS(_depthStencilBuffer.GetAddressOf())));

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = _depthStencilFormat;
	dsvDesc.Texture2D.MipSlice = 0;
	_device->CreateDepthStencilView(_depthStencilBuffer.Get(), &dsvDesc, GetDSVHandle());

	_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_depthStencilBuffer.Get(),
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));

	ThrowIfFailed(_commandList->Close());
	ID3D12CommandList* cmdsLists[] = { _commandList.Get() };
	_commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	FlushCommandQueue();

	_screenViewport.TopLeftX = 0;
	_screenViewport.TopLeftY = 0;
	_screenViewport.Width = static_cast<float>(_appDesc.clientWidth);
	_screenViewport.Height = static_cast<float>(_appDesc.clientHeight);
	_screenViewport.MinDepth = 0.0f;
	_screenViewport.MaxDepth = 1.0f;

	_scissorRect = { 0, 0, _appDesc.clientWidth, _appDesc.clientHeight };

	ENGINESTAT->ResetValues();
}


void Graphic::Update()
{
	//UniversalUtils::CalculateFrameStats();
	if (_appDesc.app != nullptr)
		_appDesc.app->Update();

	_currFrameResourceIndex = (_currFrameResourceIndex + 1) % _numFrameResources;
	_currFrameResource = _frameResources[_currFrameResourceIndex].get();

	if (_currFrameResource->fence != 0 && _fence->GetCompletedValue() < _currFrameResource->fence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		ThrowIfFailed(_fence->SetEventOnCompletion(_currFrameResource->fence, eventHandle));
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}

	_currFrameResource->Update();
}

void Graphic::RenderBegin()
{
	/* 
	* CommandList 초기화 후 렌더링에 필요한 기초 요소들 설정
	*/
	auto cmdListAlloc = _currFrameResource->cmdListAlloc;

	ThrowIfFailed(cmdListAlloc->Reset());

	ThrowIfFailed(_commandList->Reset(cmdListAlloc.Get(), RENDER->GetCurrPSO().Get()));

	_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(GetCurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	_commandList->RSSetViewports(1, &_screenViewport);
	_commandList->RSSetScissorRects(1, &_scissorRect);

	_commandList->ClearRenderTargetView(GetCurrentBackBufferView(), Colors::LightSteelBlue, 0, nullptr);
	_commandList->ClearDepthStencilView(GetDSVHandle(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	_commandList->OMSetRenderTargets(1, &GetCurrentBackBufferView(), true, &GetDSVHandle());
}


void Graphic::RenderEnd()
{
	ENGINEGUI->Render();

	_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(GetCurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	ThrowIfFailed(_commandList->Close());

	ID3D12CommandList* cmdsLists[] = { _commandList.Get() };
	_commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	ThrowIfFailed(_swapChain->Present(0, 0));
	_currBackBuffer = (_currBackBuffer + 1) % _SwapChainBufferCount;

	_currFrameResource->fence = ++_currentFence;

	_commandQueue->Signal(_fence.Get(), _currentFence);
}


// 윈도우 메세지 처리부
LRESULT Graphic::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// ImGUI test
	extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
		return true;

	AppStatus appStatus = GAMEAPP->GetAppStatus();
	switch (msg)
	{
		case WM_ACTIVATE:
			if (LOWORD(wParam) == WA_INACTIVE)
			{
				appStatus.appPaused = true;
				TIME->Stop();
			}
			else
			{
				appStatus.appPaused = false;
				TIME->Start();
			}
			GAMEAPP->SetAppStatus(appStatus);
			return 0;

		case WM_SIZE:
			_appDesc.clientWidth = LOWORD(lParam);
			_appDesc.clientHeight = HIWORD(lParam);
			if (_device)
			{
				if (wParam == SIZE_MINIMIZED)
				{
					appStatus.appPaused = true;
					appStatus.minimized = true;
					appStatus.maximized = false;
				}
				else if (wParam == SIZE_MAXIMIZED)
				{
					appStatus.appPaused = false;
					appStatus.minimized = false;
					appStatus.maximized = true;
					OnResize();
				}
				else if (wParam == SIZE_RESTORED)
				{
					if (appStatus.minimized)
					{
						appStatus.appPaused = false;
						appStatus.minimized = false;
						OnResize();
					}

					else if (appStatus.maximized)
					{
						appStatus.appPaused = false;
						appStatus.maximized = false;
						OnResize();
					}
					else if (appStatus.resizing)
					{

					}
					else
					{
						OnResize();
					}
				}
			}
			GAMEAPP->SetAppStatus(appStatus);
			return 0;

		case WM_ENTERSIZEMOVE:
			appStatus.appPaused = true;
			appStatus.resizing = true;
			TIME->Stop();
			GAMEAPP->SetAppStatus(appStatus);
			return 0;

		case WM_EXITSIZEMOVE:
			appStatus.appPaused = false;
			appStatus.resizing = false;
			TIME->Start();
			GAMEAPP->SetAppStatus(appStatus);
			OnResize();
			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		case WM_MENUCHAR:
			return MAKELRESULT(0, MNC_CLOSE);

		case WM_GETMINMAXINFO:
			((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
			((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
			return 0;

		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
			return 0;
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
			return 0;
		case WM_MOUSEMOVE:
			return 0;
		case WM_KEYUP:
			//else if ((int)wParam == VK_F2)
			//	Set4xMsaaState(!_appDesc._4xMsaaState);

			return 0;
		}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}


// 윈도우 초기화
bool Graphic::InitMainWindow()
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GAMEAPP->GetAppInst();
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"MainWnd";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	RECT R = { 0, 0, _appDesc.clientWidth, _appDesc.clientHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	_hMainWnd = CreateWindow(L"MainWnd", _appDesc.mainWndCaption.c_str(),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, GAMEAPP->GetAppInst(), 0);
	if (!_hMainWnd)
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	ShowWindow(_hMainWnd, SW_SHOW);
	UpdateWindow(_hMainWnd);

	return true;
}


// dx 초기화
bool Graphic::InitDirect3D()
{
	#if defined(DEBUG) || defined(_DEBUG) 
		{
			ComPtr<ID3D12Debug> debugController;
			ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
			debugController->EnableDebugLayer();
		}
	#endif

	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&_dxgiFactory)));

	HRESULT hardwareResult = D3D12CreateDevice(
		nullptr,
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&_device));

	if (FAILED(hardwareResult))
	{
		ComPtr<IDXGIAdapter> pWarpAdapter;
		ThrowIfFailed(_dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));

		ThrowIfFailed(D3D12CreateDevice(
			pWarpAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&_device)));
	}

	ThrowIfFailed(_device->CreateFence(0, D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(&_fence)));

	_rtvDescriptorSize = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	_dsvDescriptorSize = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	_cbvSrvUavDescriptorSize = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Format = _backBufferFormat;
	msQualityLevels.SampleCount = 4;
	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQualityLevels.NumQualityLevels = 0;
	ThrowIfFailed(_device->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&msQualityLevels,
		sizeof(msQualityLevels)));

	_appDesc._4xMsaaQuality = msQualityLevels.NumQualityLevels;
	assert(_appDesc._4xMsaaQuality > 0 && "Unexpected MSAA quality level.");

#ifdef _DEBUG
	DXUtil::LogAdapters();
#endif

	BuildCommandObjects();
	BuildSwapChain();
	BuildDescriptorHeaps();

	return true;
}

void Graphic::BuildCommandObjects()
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	ThrowIfFailed(_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&_commandQueue)));

	ThrowIfFailed(_device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(_directCmdListAlloc.GetAddressOf())));

	ThrowIfFailed(_device->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		_directCmdListAlloc.Get(),
		nullptr,
		IID_PPV_ARGS(_commandList.GetAddressOf())));

	_commandList->Close();
}

void Graphic::BuildSwapChain()
{
	_swapChain.Reset();

	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = _appDesc.clientWidth;
	sd.BufferDesc.Height = _appDesc.clientHeight;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = _backBufferFormat;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = _appDesc._4xMsaaState ? 4 : 1;
	sd.SampleDesc.Quality = _appDesc._4xMsaaState ? (_appDesc._4xMsaaQuality - 1) : 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = _SwapChainBufferCount;
	sd.OutputWindow = _hMainWnd;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	ThrowIfFailed(_dxgiFactory->CreateSwapChain(
		_commandQueue.Get(),
		&sd,
		_swapChain.GetAddressOf()));
}

void Graphic::BuildDescriptorHeaps()
{
	// Render Target View
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = _SwapChainBufferCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	ThrowIfFailed(_device->CreateDescriptorHeap(
		&rtvHeapDesc, IID_PPV_ARGS(_rtvHeap.GetAddressOf())));

	// Depth Stencil View
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 2;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	ThrowIfFailed(_device->CreateDescriptorHeap(
		&dsvHeapDesc, IID_PPV_ARGS(_dsvHeap.GetAddressOf())));
}


void Graphic::BuildFrameResources()
{
	for (int i = 0; i < _numFrameResources; ++i)
	{
		_frameResources.push_back(make_unique<FrameResource>((UINT)RENDER->GetObjects().size()));
	}
}

void Graphic::FlushCommandQueue()
{
	_currentFence++;

	ThrowIfFailed(_commandQueue->Signal(_fence.Get(), _currentFence));

	if (_fence->GetCompletedValue() < _currentFence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);

		ThrowIfFailed(_fence->SetEventOnCompletion(_currentFence, eventHandle));

		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
}
