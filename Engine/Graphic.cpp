#include "pch.h"
#include "Graphic.h"

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

HWND Graphic::GetMainWnd() const
{
	return _hMainWnd;
}

float Graphic::GetAspectRatio() const
{
	return static_cast<float>(_appDesc.clientWidth) / _appDesc.clientHeight;
}

bool Graphic::Get4xMsaaState()const
{
	return _appDesc._4xMsaaState;
}

void Graphic::Set4xMsaaState(bool value)
{
	if (_appDesc._4xMsaaState != value)
	{
		_appDesc._4xMsaaState = value;

		BuildSwapChain();
		OnResize();
	}
}

AppDesc Graphic::GetAppDesc() const
{
	return _appDesc;
}

void Graphic::SetAppDesc(AppDesc appDesc)
{
	_appDesc = appDesc;
}

int Graphic::GetNumFrameResources()const
{
	return _numFrameResources;
}

int Graphic::GetCurrFrameResourceIndex() const
{
	return _currFrameResourceIndex;
}

ComPtr<ID3D12Device> Graphic::GetDevice() const
{
	return _device;
}

ComPtr<ID3D12GraphicsCommandList> Graphic::GetCommandList()const
{
	return _commandList;
}

ComPtr<ID3D12DescriptorHeap> Graphic::GetConstantBufferHeap() const
{
	return _cbvHeap;
}

vector<unique_ptr<GameObject>>& Graphic::GetObjects()
{
	return _objects;
}

bool Graphic::Initialize()
{
	if (!InitMainWindow())
		return false;

	if (!InitDirect3D())
		return false;

	OnResize();

	ThrowIfFailed(_commandList->Reset(_directCmdListAlloc.Get(), nullptr));

	BuildRootSignature();
	BuildShaderAndInputLayout();
	BuildObjectGeometry();
	BuildFrameResources();
	BuildDescriptorHeaps();
	BuildConstantBuffers();
	BuildPSO();

	// Execute the initialization commands.
	ThrowIfFailed(_commandList->Close());
	ID3D12CommandList* cmdsLists[] = { _commandList.Get() };
	_commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Wait until initialization is complete.
	FlushCommandQueue();

	return true;
}

void Graphic::CreateRtvAndDsvDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = _SwapChainBufferCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	ThrowIfFailed(_device->CreateDescriptorHeap(
		&rtvHeapDesc, IID_PPV_ARGS(_rtvHeap.GetAddressOf())));


	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	ThrowIfFailed(_device->CreateDescriptorHeap(
		&dsvHeapDesc, IID_PPV_ARGS(_dsvHeap.GetAddressOf())));
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
	_device->CreateDepthStencilView(_depthStencilBuffer.Get(), &dsvDesc, GetDepthStencilView());

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

	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, GetAspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&_proj, P);
}


void Graphic::Update()
{
	{
		XMVECTOR pos = XMVectorSet(_cameraPos.x, _cameraPos.y, _cameraPos.z, 1.0f);
		XMVECTOR target = XMVectorZero();
		XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
		XMStoreFloat4x4(&_view, view);
	}
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

	{
		XMMATRIX view = XMLoadFloat4x4(&_view);
		XMMATRIX proj = XMLoadFloat4x4(&_proj);

		XMMATRIX viewProj = XMMatrixMultiply(view, proj);
		XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
		XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
		XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

		XMStoreFloat4x4(&_mainPassCB.View, XMMatrixTranspose(view));
		XMStoreFloat4x4(&_mainPassCB.InvView, XMMatrixTranspose(invView));
		XMStoreFloat4x4(&_mainPassCB.Proj, XMMatrixTranspose(proj));
		XMStoreFloat4x4(&_mainPassCB.InvProj, XMMatrixTranspose(invProj));
		XMStoreFloat4x4(&_mainPassCB.ViewProj, XMMatrixTranspose(viewProj));
		XMStoreFloat4x4(&_mainPassCB.InvViewProj, XMMatrixTranspose(invViewProj));
		_mainPassCB.EyePosW = _eyePos;
		_mainPassCB.RenderTargetSize = XMFLOAT2((float)_appDesc.clientWidth, (float)_appDesc.clientHeight);
		_mainPassCB.InvRenderTargetSize = XMFLOAT2(1.0f / _appDesc.clientWidth, 1.0f / _appDesc.clientHeight);
		_mainPassCB.NearZ = 1.0f;
		_mainPassCB.FarZ = 1000.0f;
		_mainPassCB.TotalTime = TIME->TotalTime();
		_mainPassCB.DeltaTime = TIME->DeltaTime();

		auto currPassCB = _currFrameResource->passCB.get();
		currPassCB->CopyData(0, _mainPassCB);
		//ObjectConstants objConstants;
		//XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(worldViewProj));
		//_objectCB->CopyData(0, objConstants);
	}
}


void Graphic::Render()
{
	RenderBegin();

	//================
	ID3D12DescriptorHeap* descriptorHeaps[] = { _cbvHeap.Get() };
	_commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	_commandList->SetGraphicsRootSignature(_rootSignature.Get());

	int passCbvIndex = _passCbvOffset + _currFrameResourceIndex;
	auto passCbvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(_cbvHeap->GetGPUDescriptorHandleForHeapStart());
	passCbvHandle.Offset(passCbvIndex, _cbvSrvUavDescriptorSize);
	_commandList->SetGraphicsRootDescriptorTable(1, passCbvHandle);

	for (int i = 0; i < _objects.size(); i++)
		_objects[i]->Render();

	//=================

	RenderEnd();
}


void Graphic::RenderBegin()
{
	/* 
	* CommandList 초기화 후 렌더링에 필요한 기초 요소들 설정
	*/
	auto cmdListAlloc = _currFrameResource->cmdListAlloc;

	ThrowIfFailed(cmdListAlloc->Reset());

	ThrowIfFailed(_commandList->Reset(cmdListAlloc.Get(), _PSOs["opaque"].Get()));

	_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(GetCurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	_commandList->RSSetViewports(1, &_screenViewport);
	_commandList->RSSetScissorRects(1, &_scissorRect);

	_commandList->ClearRenderTargetView(GetCurrentBackBufferView(), Colors::LightSteelBlue, 0, nullptr);
	_commandList->ClearDepthStencilView(GetDepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	_commandList->OMSetRenderTargets(1, &GetCurrentBackBufferView(), true, &GetDepthStencilView());
}


void Graphic::RenderEnd()
{
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
			if (wParam == VK_ESCAPE)
			{
				PostQuitMessage(0);
			}
			else if ((int)wParam == VK_F2)
				Set4xMsaaState(!_appDesc._4xMsaaState);

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
	LogAdapters();
#endif

	BuildCommandObjects();
	BuildSwapChain();
	CreateRtvAndDsvDescriptorHeaps();

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
	UINT objCount = (UINT)_objects.size();

	UINT numDescriptors = (objCount + 1) * _numFrameResources;

	_passCbvOffset = objCount * _numFrameResources;

	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	cbvHeapDesc.NumDescriptors = numDescriptors;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;
	ThrowIfFailed(_device->CreateDescriptorHeap(&cbvHeapDesc,
		IID_PPV_ARGS(&_cbvHeap)));
}

void Graphic::BuildConstantBuffers()
{
	//_objectCB = make_unique<UploadBuffer<ObjectConstants>>(_device.Get(), 1, true);

	UINT objCBByteSize = DXUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));

	UINT objCount = (UINT)_objects.size();

	for (int frameIndex = 0; frameIndex < _numFrameResources; ++frameIndex)
	{
		auto objectCB = _frameResources[frameIndex]->objectCB->GetResource();
		for (UINT i = 0; i < objCount; ++i)
		{
			D3D12_GPU_VIRTUAL_ADDRESS cbAddress = objectCB->GetGPUVirtualAddress();

			cbAddress += i * objCBByteSize;

			int heapIndex = frameIndex * objCount + i;
			auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(_cbvHeap->GetCPUDescriptorHandleForHeapStart());
			handle.Offset(heapIndex, _cbvSrvUavDescriptorSize);

			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
			cbvDesc.BufferLocation = cbAddress;
			cbvDesc.SizeInBytes = objCBByteSize;

			_device->CreateConstantBufferView(&cbvDesc, handle);
		}
	}

	UINT passCBByteSize = DXUtil::CalcConstantBufferByteSize(sizeof(PassConstants));
	
	for (int frameIndex = 0; frameIndex < _numFrameResources; ++frameIndex)
	{
		auto passCB = _frameResources[frameIndex]->passCB->GetResource();
		D3D12_GPU_VIRTUAL_ADDRESS cbAddress = passCB->GetGPUVirtualAddress();

		// Offset to the pass cbv in the descriptor heap.
		int heapIndex = _passCbvOffset + frameIndex;
		auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(_cbvHeap->GetCPUDescriptorHandleForHeapStart());
		handle.Offset(heapIndex, _cbvSrvUavDescriptorSize);

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
		cbvDesc.BufferLocation = cbAddress;
		cbvDesc.SizeInBytes = passCBByteSize;

		_device->CreateConstantBufferView(&cbvDesc, handle);
	}
}

void Graphic::BuildRootSignature()
{
	CD3DX12_ROOT_PARAMETER slotRootParameter[2];

	CD3DX12_DESCRIPTOR_RANGE cbvTable0;
	cbvTable0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	CD3DX12_DESCRIPTOR_RANGE cbvTable1;
	cbvTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);

	slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable0);
	slotRootParameter[1].InitAsDescriptorTable(1, &cbvTable1);

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(2, slotRootParameter, 0, nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}
	ThrowIfFailed(hr);

	ThrowIfFailed(_device->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(&_rootSignature)));
}

void Graphic::BuildShaderAndInputLayout()
{
	HRESULT hr = S_OK;

	_shaders["standardVS"] = DXUtil::CompileShader(L"Shaders\\color.hlsl", nullptr, "VS", "vs_5_1");
	_shaders["opaquePS"] = DXUtil::CompileShader(L"Shaders\\color.hlsl", nullptr, "PS", "ps_5_1");

	_inputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
}

void Graphic::BuildObjectGeometry()
{
	Mesh boxMesh = GeometryGenerator::CreateBox(1.5f, 0.5f, 1.5f, 3);
	unique_ptr<Geometry> geo = Geometry::CreateGeometry(boxMesh, "BasicShapeGeo", "box");
	_geometrys[geo->name] = move(geo);

	//================

	auto box = make_unique<GameObject>();
	box = make_unique<GameObject>();
	box->objCBIndex = _objects.size();
	box->geometry = _geometrys["BasicShapeGeo"].get();
	box->primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	box->meshName = "box";
	box->indexCount = box->geometry->drawArgs["box"].indexCount;
	box->startIndexLocation = box->geometry->drawArgs["box"].startIndexLocation;
	box->baseVertexLocation = box->geometry->drawArgs["box"].baseVertexLocation;
	XMStoreFloat4x4(&box->world, XMMatrixTranslation(0.0f, -10.0f, 10.0f));
	_objects.push_back(move(box));
}

void Graphic::BuildPSO()
{
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
		ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
		psoDesc.InputLayout = { _inputLayout.data(), (UINT)_inputLayout.size() };
		psoDesc.pRootSignature = _rootSignature.Get();
		psoDesc.VS =
		{
			reinterpret_cast<BYTE*>(_shaders["standardVS"]->GetBufferPointer()),
			_shaders["standardVS"]->GetBufferSize()
		};
		psoDesc.PS =
		{
			reinterpret_cast<BYTE*>(_shaders["opaquePS"]->GetBufferPointer()),
			_shaders["opaquePS"]->GetBufferSize()
		};
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = _backBufferFormat;
		psoDesc.SampleDesc.Count = _appDesc._4xMsaaState ? 4 : 1;
		psoDesc.SampleDesc.Quality = _appDesc._4xMsaaState ? (_appDesc._4xMsaaQuality - 1) : 0;
		psoDesc.DSVFormat = _depthStencilFormat;
		ThrowIfFailed(_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&_PSOs["opaque"])));
	}
}

void Graphic::BuildFrameResources()
{
	for (int i = 0; i < _numFrameResources; ++i)
	{
		_frameResources.push_back(make_unique<FrameResource>(_device.Get(), 1,
			(UINT)_objects.size()));
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

ID3D12Resource* Graphic::GetCurrentBackBuffer()const
{
	return _swapChainBuffer[_currBackBuffer].Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE Graphic::GetCurrentBackBufferView()const
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		_rtvHeap->GetCPUDescriptorHandleForHeapStart(),
		_currBackBuffer,
		_rtvDescriptorSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE Graphic::GetDepthStencilView()const
{
	return _dsvHeap->GetCPUDescriptorHandleForHeapStart();
}

void Graphic::CalculateFrameStats()
{
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	if ((TIME->TotalTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt;
		float mspf = 1000.0f / fps;

		wstring fpsStr = to_wstring(fps);
		wstring mspfStr = to_wstring(mspf);

		wstring windowText = _appDesc.mainWndCaption +
			L"    fps: " + fpsStr +
			L"   mspf: " + mspfStr;

		SetWindowText(_hMainWnd, windowText.c_str());

		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}

void Graphic::LogAdapters()
{
	UINT i = 0;
	IDXGIAdapter* adapter = nullptr;
	vector<IDXGIAdapter*> adapterList;
	while (_dxgiFactory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC desc;
		adapter->GetDesc(&desc);

		wstring text = L"***Adapter: ";
		text += desc.Description;
		text += L"\n";

		OutputDebugString(text.c_str());

		adapterList.push_back(adapter);

		++i;
	}

	for (size_t i = 0; i < adapterList.size(); ++i)
	{
		LogAdapterOutputs(adapterList[i]);
		ReleaseCom(adapterList[i]);
	}
}

void Graphic::LogAdapterOutputs(IDXGIAdapter* adapter)
{
	UINT i = 0;
	IDXGIOutput* output = nullptr;
	while (adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_OUTPUT_DESC desc;
		output->GetDesc(&desc);

		wstring text = L"***Output: ";
		text += desc.DeviceName;
		text += L"\n";
		OutputDebugString(text.c_str());

		LogOutputDisplayModes(output, _backBufferFormat);

		ReleaseCom(output);

		++i;
	}
}

void Graphic::LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format)
{
	UINT count = 0;
	UINT flags = 0;

	output->GetDisplayModeList(format, flags, &count, nullptr);

	vector<DXGI_MODE_DESC> modeList(count);
	output->GetDisplayModeList(format, flags, &count, &modeList[0]);

	for (auto& x : modeList)
	{
		UINT n = x.RefreshRate.Numerator;
		UINT d = x.RefreshRate.Denominator;
		wstring text =
			L"Width = " + to_wstring(x.Width) + L" " +
			L"Height = " + to_wstring(x.Height) + L" " +
			L"Refresh = " + to_wstring(n) + L"/" + to_wstring(d) +
			L"\n";

		::OutputDebugString(text.c_str());
	}
}