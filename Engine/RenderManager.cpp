#include "pch.h"
#include "RenderManager.h"

void RenderManager::Init()
{
	BuildRootSignature();
	BuildInputLayout();
	BuildSRVDescriptorHeap();
	RESOURCE->CreateDefaultResources();

	{
		auto opaqueSolid = CreatePSODesc(_solidInputLayout, L"standardVS", L"opaquePS");
		auto opaqueSkinned = CreatePSODesc(_skinnedInputLayout, L"skinnedVS", L"opaquePS");
		
		auto opaqueWireframe = opaqueSkinned;
		opaqueWireframe.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
		
		auto skybox = CreatePSODesc(_solidInputLayout, L"skyboxVS", L"skyboxPS");
		skybox.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		skybox.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

		auto debug = CreatePSODesc(_colliderDebugInputLayout, L"debugVS", L"debugPS");
		debug.DepthStencilState.DepthEnable = false;
		debug.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		debug.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		debug.DepthStencilState.StencilEnable = false;
		debug.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		debug.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;

		auto shadow = CreatePSODesc(_solidInputLayout, L"shadowVS", L"shadowPS");
		shadow.RTVFormats[0] = DXGI_FORMAT_UNKNOWN; // 깊이만
		shadow.NumRenderTargets = 0;
		shadow.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		shadow.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		shadow.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		shadow.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		shadow.RasterizerState.SlopeScaledDepthBias = 0.0f;

		auto skinnedShadow = CreatePSODesc(_skinnedInputLayout, L"skinnedShadowVS", L"shadowPS");
		skinnedShadow.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
		skinnedShadow.NumRenderTargets = 0;
		skinnedShadow.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		skinnedShadow.RasterizerState = shadow.RasterizerState;

		auto shadowDebug = CreatePSODesc(_shadowDebugInputLayout, L"shadowDebugVS", L"shadowDebugPS");
		shadowDebug.DepthStencilState.DepthEnable = FALSE;

		BuildPSO(PSO_OPAQUE_SOLID, opaqueSolid);
		BuildPSO(PSO_OPAQUE_SKINNED, opaqueSkinned);
		BuildPSO(PSO_WIREFRAME, opaqueWireframe);
		BuildPSO(PSO_SKYBOX, skybox);
		BuildPSO(PSO_SHADOWMAP, shadow);
		BuildPSO(PSO_SHADOWMAP_SKINNED, skinnedShadow);

		BuildPSO(PSO_DEBUG_PHYSICS, debug);
		BuildPSO(PSO_DEBUG_SHADOW, shadowDebug);
		SetDefaultPSO();
	}

	_materialCB = make_unique<UploadBuffer<MaterialConstants>>(CB_COUNT_MATERIAL, true);
	_cameraCB = make_unique<UploadBuffer<CameraConstants>>(1, true);
	
	_shadowMap = make_unique<ShadowMap>(2048, 2048);
	_shadowMap->BuildDescriptors();
}

void RenderManager::FixedUpdate()
{
	for (auto& o : _objects)
		o->FixedUpdate();
}

void RenderManager::Update()
{
	for (auto& o : _objects)
		o->Update();

	UpdateMaterialCB();
	UpdateCameraCB();
}

void RenderManager::Render()
{
	auto cmdList = GRAPHIC->GetCommandList();
	ID3D12DescriptorHeap* descriptorHeaps[] = { _srvHeap.Get() };
	cmdList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	cmdList->SetGraphicsRootSignature(_rootSignature.Get());

	auto lightSB = GRAPHIC->GetCurrFrameResource()->lightSB->GetResource();
	CD3DX12_GPU_DESCRIPTOR_HANDLE lightDesc(RENDER->GetShaderResourceViewHeap()->GetGPUDescriptorHandleForHeapStart());
	lightDesc.Offset(GRAPHIC->GetCurrFrameResource()->GetLightSRVHeapIndex(), GRAPHIC->GetCBVSRVDescriptorSize());

	cmdList->SetGraphicsRootDescriptorTable(ROOT_PARAMETER_SHADOWTEX_SR, _shadowMap->GetSrv());
	cmdList->SetGraphicsRootDescriptorTable(ROOT_PARAMETER_LIGHT_CB, lightDesc);
	cmdList->SetGraphicsRoot32BitConstant(ROOT_PARAMETER_LIGHTINFO_CB, _lights.size(), 0);
	cmdList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_CAMERA_CB, _cameraCB->GetResource()->GetGPUVirtualAddress());

	// ShadowMap Pass
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_shadowMap->GetResource(),
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE));

	cmdList->RSSetViewports(1, &_shadowMap->GetViewport());
	cmdList->RSSetScissorRects(1, &_shadowMap->GetScissorRect());

	cmdList->ClearDepthStencilView(_shadowMap->GetDsv(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	cmdList->OMSetRenderTargets(0, nullptr, false, &_shadowMap->GetDsv());

	cmdList->SetPipelineState(_PSOs[PSO_SHADOWMAP].Get());

	for (auto& p : _sortedObjects)
	{
		if (p.first == PSO_SKYBOX || p.first == PSO_OPAQUE_SKINNED)
			continue;

		for (int i = 0; i < p.second.size(); i++)
			p.second[i]->Render();
	}

	cmdList->SetPipelineState(_PSOs[PSO_SHADOWMAP_SKINNED].Get());

	for (auto& p : _sortedObjects)
	{
		if (p.first == PSO_SKYBOX || p.first == PSO_OPAQUE_SOLID)
			continue;

		for (int i = 0; i < p.second.size(); i++)
			p.second[i]->Render();
	}

	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_shadowMap->GetResource(),
		D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ));

	// Main Pass
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(GRAPHIC->GetCurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	cmdList->RSSetViewports(1, &GRAPHIC->GetViewport());
	cmdList->RSSetScissorRects(1, &GRAPHIC->GetScissorRect());

	cmdList->ClearRenderTargetView(GRAPHIC->GetCurrentBackBufferView(), Colors::LightSteelBlue, 0, nullptr);
	cmdList->ClearDepthStencilView(GRAPHIC->GetDSVHandle(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	cmdList->OMSetRenderTargets(1, &GRAPHIC->GetCurrentBackBufferView(), true, &GRAPHIC->GetDSVHandle());

	// 이거 최적화할 필요 있을듯
	for (auto& p : _sortedObjects)
	{
		if (_isPSOFixed && p.first != PSO_SKYBOX)
			cmdList->SetPipelineState(_currPSO.Get());
		else
			cmdList->SetPipelineState(_PSOs[p.first].Get());

		for (int i = 0; i < p.second.size(); i++)
			p.second[i]->Render();
	}

	if (_isPhysicsDebugRenderEnabled)
	{
		cmdList->SetPipelineState(_PSOs[PSO_DEBUG_PHYSICS].Get());
		DEBUG->Render();
	}
}

D3D12_GRAPHICS_PIPELINE_STATE_DESC RenderManager::CreatePSODesc(vector<D3D12_INPUT_ELEMENT_DESC>& inputLayout, wstring vsName, wstring psName, wstring dsName, wstring hsName, wstring gsName)
{
	AppDesc appDesc = GRAPHIC->GetAppDesc();
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.InputLayout = { inputLayout.data(), (UINT)inputLayout.size() };
	psoDesc.pRootSignature = _rootSignature.Get();

	// VS, PS is necessary!!!!!!!!
	auto vertexShader = RESOURCE->Get<Shader>(vsName)->GetBlob();
	psoDesc.VS =
	{
		reinterpret_cast<BYTE*>(vertexShader->GetBufferPointer()),
		vertexShader->GetBufferSize()
	};

	auto pixelShader = RESOURCE->Get<Shader>(psName)->GetBlob();
	psoDesc.PS = 
	{
		reinterpret_cast<BYTE*>(pixelShader->GetBufferPointer()),
		pixelShader->GetBufferSize()
	};

	if (!dsName.empty())
	{
		auto domainShader = RESOURCE->Get<Shader>(dsName)->GetBlob();
		psoDesc.DS =
		{
			reinterpret_cast<BYTE*>(domainShader->GetBufferPointer()),
			domainShader->GetBufferSize()
		};
	}

	if (!hsName.empty())
	{
		auto hullShader = RESOURCE->Get<Shader>(hsName)->GetBlob();
		psoDesc.HS =
		{
			reinterpret_cast<BYTE*>(hullShader->GetBufferPointer()),
			hullShader->GetBufferSize()
		};
	}

	if (!gsName.empty())
	{
		auto geoShader = RESOURCE->Get<Shader>(gsName)->GetBlob();
		psoDesc.GS =
		{
			reinterpret_cast<BYTE*>(geoShader->GetBufferPointer()),
			geoShader->GetBufferSize()
		};
	}

	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = GRAPHIC->GetBackBufferFormat();
	psoDesc.SampleDesc.Count = appDesc._4xMsaaState ? 4 : 1;
	psoDesc.SampleDesc.Quality = appDesc._4xMsaaState ? (appDesc._4xMsaaQuality - 1) : 0;
	psoDesc.DSVFormat = GRAPHIC->GetDepthStencilFormat();

	return psoDesc;
}


#pragma region Setters

void RenderManager::SetCurrPSO(string name)
{
	_currPSO = _PSOs[name];
	_isPSOFixed = true;
}

void RenderManager::SetDefaultPSO()
{
	_currPSO = _PSOs[PSO_OPAQUE_SOLID];
	_isPSOFixed = false;
}

#pragma endregion

void RenderManager::UpdateObjectPSO(shared_ptr<GameObject> obj, string targetPSO)
{
	for (int i = 0; i < _sortedObjects[obj->GetPSOName()].size(); i++)
	{
		if (_sortedObjects[obj->GetPSOName()][i] == obj)
		{
			_sortedObjects[obj->GetPSOName()].erase(_sortedObjects[obj->GetPSOName()].begin() + i);
			_sortedObjects[targetPSO].push_back(obj);
			break;
		}
	}
}

shared_ptr<GameObject> RenderManager::AddGameObject(shared_ptr<GameObject> obj)
{
	for (auto& o : _objects)
	{
		if (obj == o)
			return nullptr;
	}
	obj->objCBIndex = _objects.size();
	obj->primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	_sortedObjects[obj->GetPSOName()].push_back(obj);
	_objects.push_back(move(obj));
	return _objects[_objects.size() - 1];
}


#pragma region Build_Render_Components

void RenderManager::BuildPSO(string name, D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc)
{
	ThrowIfFailed(GRAPHIC->GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(_PSOs[name].GetAddressOf())));
}

void RenderManager::BuildRootSignature()
{
	CD3DX12_DESCRIPTOR_RANGE cubemapTable;
	cubemapTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	CD3DX12_DESCRIPTOR_RANGE texTable;
	texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
	CD3DX12_DESCRIPTOR_RANGE shadowTexTable;
	shadowTexTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);
	CD3DX12_DESCRIPTOR_RANGE lightTable;
	lightTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3);
	CD3DX12_DESCRIPTOR_RANGE boneTable;
	boneTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 1);


	CD3DX12_ROOT_PARAMETER slotRootParameter[9];

	slotRootParameter[ROOT_PARAMETER_SKYBOX_SR].InitAsDescriptorTable(1, &cubemapTable, D3D12_SHADER_VISIBILITY_PIXEL);		// CubemapSR
	slotRootParameter[ROOT_PARAMETER_TEXTURE_SR].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);		// TextureSR
	slotRootParameter[ROOT_PARAMETER_SHADOWTEX_SR].InitAsDescriptorTable(1, &shadowTexTable, D3D12_SHADER_VISIBILITY_PIXEL);
	slotRootParameter[ROOT_PARAMETER_BONE_SB].InitAsDescriptorTable(1, &boneTable, D3D12_SHADER_VISIBILITY_VERTEX);			// BoneSB
	slotRootParameter[ROOT_PARAMETER_LIGHT_CB].InitAsDescriptorTable(1, &lightTable);		// LightSB

	slotRootParameter[ROOT_PARAMETER_LIGHTINFO_CB].InitAsConstantBufferView(0);	// LightInfoCB
	slotRootParameter[ROOT_PARAMETER_OBJECT_CB].InitAsConstantBufferView(1);	// ObjectCB
	slotRootParameter[ROOT_PARAMETER_MATERIAL_CB].InitAsConstantBufferView(2);	// MaterialCB
	slotRootParameter[ROOT_PARAMETER_CAMERA_CB].InitAsConstantBufferView(3);	// CameraCB

	auto staticSamplers = GetStaticSamplers();

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(9, slotRootParameter, (UINT)staticSamplers.size(), staticSamplers.data(),
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

	ThrowIfFailed(GRAPHIC->GetDevice()->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(_rootSignature.GetAddressOf())));
}

void RenderManager::BuildInputLayout()
{
	_solidInputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	_skinnedInputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 44, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_SINT, 0, 60, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	_colliderDebugInputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	_shadowDebugInputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
}

void RenderManager::BuildSRVDescriptorHeap()
{
	// Descriptor의 갯수를 50으로 고정 제한중인데 유동적으로 가변으로 바꾸는 작업 고려
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 150;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	ThrowIfFailed(GRAPHIC->GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&_srvHeap)));
}

#pragma endregion


#pragma region Update_Constant_Buffers

void RenderManager::UpdateMaterialCB()
{
	auto materials = RESOURCE->GetByType<Material>();
	for (auto& m : materials)
	{
		shared_ptr<Material> mat = static_pointer_cast<Material>(m.second);
		if (mat->numFramesDirty > 0)
		{
			XMMATRIX matTransform = XMLoadFloat4x4(&mat->matTransform);

			MaterialConstants matConstants;
			XMStoreFloat4x4(&matConstants.matTransform, XMMatrixTranspose(matTransform));
			matConstants.Ambient = mat->ambient;
			matConstants.Diffuse = mat->diffuse;
			matConstants.Specular = mat->specular;
			matConstants.Emissive = mat->emissive;
			matConstants.Tilling = mat->tilling;
			matConstants.Shiness = mat->shiness;

			_materialCB->CopyData(mat->matCBIndex, matConstants);

			mat->numFramesDirty--;
		}
	}
}

void RenderManager::UpdateCameraCB()
{
	XMMATRIX view = XMLoadFloat4x4(&Camera::GetViewMatrix());
	XMMATRIX proj = XMLoadFloat4x4(&Camera::GetProjMatrix());
	XMMATRIX viewProj = XMMatrixMultiply(view, proj);

	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
	XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
	XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

	XMStoreFloat4x4(&_cameraConstants.View, XMMatrixTranspose(view));
	XMStoreFloat4x4(&_cameraConstants.InvView, XMMatrixTranspose(invView));
	XMStoreFloat4x4(&_cameraConstants.Proj, XMMatrixTranspose(proj));
	XMStoreFloat4x4(&_cameraConstants.InvProj, XMMatrixTranspose(invProj));
	XMStoreFloat4x4(&_cameraConstants.ViewProj, XMMatrixTranspose(viewProj));
	XMStoreFloat4x4(&_cameraConstants.InvViewProj, XMMatrixTranspose(invViewProj));

	_cameraConstants.RenderTargetSize = XMFLOAT2((float)GRAPHIC->GetAppDesc().clientWidth, (float)GRAPHIC->GetAppDesc().clientHeight);
	_cameraConstants.InvRenderTargetSize = XMFLOAT2(1.0f / GRAPHIC->GetAppDesc().clientWidth, 1.0f / GRAPHIC->GetAppDesc().clientHeight);

	_cameraCB->CopyData(0, _cameraConstants);
}

#pragma endregion


std::array<const CD3DX12_STATIC_SAMPLER_DESC, STATIC_SAMPLER_COUNT> RenderManager::GetStaticSamplers()
{
	// Applications usually only need a handful of samplers.  So just define them all up front
	// and keep them available as part of the root signature.  

	const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
		0, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
		1, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
		2, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
		3, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(
		4, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressW
		0.0f,                             // mipLODBias
		8);                               // maxAnisotropy

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(
		5, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressW
		0.0f,                              // mipLODBias
		8);                                // maxAnisotropy

	return {
		pointWrap, pointClamp,
		linearWrap, linearClamp,
		anisotropicWrap, anisotropicClamp };
}
