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
		{
			debug.DepthStencilState.DepthEnable = false;
			debug.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
			debug.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
			debug.DepthStencilState.StencilEnable = false;
			debug.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
			debug.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		}

		auto shadow = CreatePSODesc(_solidInputLayout, L"shadowVS", L"shadowPS");
		{
			shadow.RTVFormats[0] = DXGI_FORMAT_UNKNOWN; // ±Ì¿Ã∏∏
			shadow.NumRenderTargets = 0;
			shadow.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
			shadow.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
			shadow.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
			shadow.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
			shadow.RasterizerState.SlopeScaledDepthBias = 0.0f;
		}

		auto skinnedShadow = CreatePSODesc(_skinnedInputLayout, L"skinnedShadowVS", L"shadowPS");
		{
			skinnedShadow.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
			skinnedShadow.NumRenderTargets = 0;
			skinnedShadow.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
			skinnedShadow.RasterizerState = shadow.RasterizerState;;
		}

		auto particleUpdate = CreateCSPSODesc(L"particleCS");
		auto particleRender = CreatePSODesc(L"particleVS", L"particlePS", L"", L"", L"particleGS");
		{
			particleRender.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;

			D3D12_BLEND_DESC blendDesc = {};
			blendDesc.AlphaToCoverageEnable = FALSE;
			blendDesc.IndependentBlendEnable = FALSE;
			auto& blendRt = blendDesc.RenderTarget[0];
			blendRt.BlendEnable = TRUE;
			blendRt.LogicOpEnable = FALSE;
			blendRt.SrcBlend = D3D12_BLEND_SRC_ALPHA;
			blendRt.DestBlend = D3D12_BLEND_ONE;
			blendRt.BlendOp = D3D12_BLEND_OP_ADD;
			blendRt.SrcBlendAlpha = D3D12_BLEND_ONE;
			blendRt.DestBlendAlpha = D3D12_BLEND_ZERO;
			blendRt.BlendOpAlpha = D3D12_BLEND_OP_ADD;
			blendRt.LogicOp = D3D12_LOGIC_OP_NOOP;
			blendRt.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

			D3D12_DEPTH_STENCIL_DESC depthDesc = {};
			depthDesc.DepthEnable = TRUE;
			depthDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
			depthDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

			particleRender.BlendState = blendDesc;
			particleRender.DepthStencilState = depthDesc;
		}

		BuildPSO(PSO_OPAQUE_SOLID, opaqueSolid);
		BuildPSO(PSO_OPAQUE_SKINNED, opaqueSkinned);
		BuildPSO(PSO_WIREFRAME, opaqueWireframe);
		BuildPSO(PSO_SKYBOX, skybox);
		BuildPSO(PSO_SHADOWMAP, shadow);
		BuildPSO(PSO_SHADOWMAP_SKINNED, skinnedShadow);
		BuildPSO(PSO_DEBUG_PHYSICS, debug);
		BuildPSO(PSO_PARTICLE_UPDATE, particleUpdate);
		BuildPSO(PSO_PARTICLE_RENDER, particleRender);
		SetDefaultPSO();
	}
	
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
}

void RenderManager::Render()
{
	auto cmdList = GRAPHIC->GetCommandList();
	auto currentFrameResource = GRAPHIC->GetCurrFrameResource();

	ID3D12DescriptorHeap* descriptorHeaps[] = { _srvHeap.Get() };
	cmdList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	cmdList->SetGraphicsRootSignature(_rootSignature.Get());
	cmdList->SetComputeRootSignature(RENDER->GetRootSignature().Get());

	float clientInfo[2] = { TIME->DeltaTime(), TIME->TotalTime() };
	cmdList->SetGraphicsRoot32BitConstants(ROOT_PARAM_CLIENTINFO_C, 2, &clientInfo, 0);
	cmdList->SetComputeRoot32BitConstants(ROOT_PARAM_CLIENTINFO_C, 2, &clientInfo, 0);

	CD3DX12_GPU_DESCRIPTOR_HANDLE instance(RENDER->GetCommonSRVHeap()->GetGPUDescriptorHandleForHeapStart());
	instance.Offset(currentFrameResource->GetInstanceSRVHeapIndex(), GRAPHIC->GetCBVSRVDescriptorSize());
	cmdList->SetGraphicsRootDescriptorTable(ROOT_PARAM_INSTCANCE_SB, instance);

	CD3DX12_GPU_DESCRIPTOR_HANDLE mat(RENDER->GetCommonSRVHeap()->GetGPUDescriptorHandleForHeapStart());
	mat.Offset(currentFrameResource->GetMaterialSRVHeapIndex(), GRAPHIC->GetCBVSRVDescriptorSize());
	cmdList->SetGraphicsRootDescriptorTable(ROOT_PARAM_MATERIAL_SB, mat);

	CD3DX12_GPU_DESCRIPTOR_HANDLE lightDesc(GetCommonSRVHeap()->GetGPUDescriptorHandleForHeapStart());
	lightDesc.Offset(currentFrameResource->GetLightSRVHeapIndex(), GRAPHIC->GetCBVSRVDescriptorSize());
	cmdList->SetGraphicsRootDescriptorTable(ROOT_PARAM_LIGHT_SB, lightDesc);

	CD3DX12_GPU_DESCRIPTOR_HANDLE skybox(RENDER->GetCommonSRVHeap()->GetGPUDescriptorHandleForHeapStart());
	skybox.Offset(_skyboxTexSrvHeapIndex, GRAPHIC->GetCBVSRVDescriptorSize());
	cmdList->SetGraphicsRootDescriptorTable(ROOT_PARAM_SKYBOX_SR, skybox);

	cmdList->SetGraphicsRootDescriptorTable(ROOT_PARAM_SHADOWMAP_SR, _shadowMap->GetSrv());

	CD3DX12_GPU_DESCRIPTOR_HANDLE tex(RENDER->GetCommonSRVHeap()->GetGPUDescriptorHandleForHeapStart());
	tex.Offset(0, GRAPHIC->GetCBVSRVDescriptorSize());
	cmdList->SetGraphicsRootDescriptorTable(ROOT_PARAM_TEXTURE_ARR, tex);

	cmdList->SetGraphicsRoot32BitConstant(ROOT_PARAM_LIGHTINFO_C, _lights.size(), 0);

	cmdList->SetGraphicsRootConstantBufferView(ROOT_PARAM_CAMERA_CB, currentFrameResource->cameraCB->GetResource()->GetGPUVirtualAddress());

	// ShadowMap Pass
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_shadowMap->GetResource(),
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE));

	cmdList->RSSetViewports(1, &_shadowMap->GetViewport());
	cmdList->RSSetScissorRects(1, &_shadowMap->GetScissorRect());

	cmdList->ClearDepthStencilView(_shadowMap->GetDsv(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	cmdList->OMSetRenderTargets(0, nullptr, false, &_shadowMap->GetDsv());

	// Shadow Map Solid
	cmdList->SetPipelineState(_PSOs[PSO_SHADOWMAP].Get());

	for (auto& o : _sortedObjects[PSO_OPAQUE_SOLID]) {
		o->Render();
	}

	// Shadow Map Skinned
	cmdList->SetPipelineState(_PSOs[PSO_SHADOWMAP_SKINNED].Get());

	for (auto& o : _sortedObjects[PSO_OPAQUE_SKINNED]) {
		o->Render();
	}

	RefreshMeshRenderCheckMap();

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

	// Skybox
	if (_sortedObjects[PSO_SKYBOX].size() > 0) {
		cmdList->SetPipelineState(_PSOs[PSO_SKYBOX].Get());
		for (auto& o : _sortedObjects[PSO_SKYBOX]) {
			o->Render();
		}
	}

	// Check PSO Fixed
	if (!_isPSOFixed) {
		// Opaque Solid
		if (_sortedObjects[PSO_OPAQUE_SOLID].size() > 0) {
			cmdList->SetPipelineState(_PSOs[PSO_OPAQUE_SOLID].Get());
			for (auto& o : _sortedObjects[PSO_OPAQUE_SOLID]) {
				o->Render();
			}
		}

		// Opaque Skinned
		if (_sortedObjects[PSO_OPAQUE_SKINNED].size() > 0) {
			cmdList->SetPipelineState(_PSOs[PSO_OPAQUE_SKINNED].Get());
			for (auto& o : _sortedObjects[PSO_OPAQUE_SKINNED]) {
				o->Render();
			}
		}
	}
	else {
		cmdList->SetPipelineState(_currPSO.Get());
		for (auto& p : _sortedObjects) {
			if (p.first != PSO_SKYBOX) {
				for (auto& o : p.second) {
					o->Render();
				}
			}
		}
	}

	RefreshMeshRenderCheckMap();

	if (_isPhysicsDebugRenderEnabled)
	{
		cmdList->SetPipelineState(_PSOs[PSO_DEBUG_PHYSICS].Get());
		DEBUG->Render();
	}

	PARTICLE->Update();

	cmdList->SetPipelineState(_PSOs[PSO_PARTICLE_RENDER].Get());
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	PARTICLE->Render();
}

D3D12_GRAPHICS_PIPELINE_STATE_DESC RenderManager::CreatePSODesc(vector<D3D12_INPUT_ELEMENT_DESC>& inputLayout, wstring vsName, wstring psName, wstring dsName, wstring hsName, wstring gsName)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = CreatePSODesc(vsName, psName, dsName, hsName, gsName);
	psoDesc.InputLayout = { inputLayout.data(), (UINT)inputLayout.size() };

	return psoDesc;
}


D3D12_GRAPHICS_PIPELINE_STATE_DESC RenderManager::CreatePSODesc(wstring vsName, wstring psName, wstring dsName, wstring hsName, wstring gsName)
{
	AppDesc appDesc = GRAPHIC->GetAppDesc();
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	psoDesc.pRootSignature = _rootSignature.Get();

	if (!vsName.empty())
	{
		auto vertexShader = RESOURCE->Get<Shader>(vsName)->GetBlob();
		psoDesc.VS =
		{
			reinterpret_cast<BYTE*>(vertexShader->GetBufferPointer()),
			vertexShader->GetBufferSize()
		};
	}

	if (!psName.empty())
	{
		auto pixelShader = RESOURCE->Get<Shader>(psName)->GetBlob();
		psoDesc.PS =
		{
			reinterpret_cast<BYTE*>(pixelShader->GetBufferPointer()),
			pixelShader->GetBufferSize()
		};
	}

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

D3D12_COMPUTE_PIPELINE_STATE_DESC RenderManager::CreateCSPSODesc(wstring csName)
{
	D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature = _rootSignature.Get();
	auto computeShader = RESOURCE->Get<Shader>(csName)->GetBlob();
	psoDesc.CS =
	{
		reinterpret_cast<BYTE*>(computeShader->GetBufferPointer()),
		computeShader->GetBufferSize()
	};
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

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
	obj->objectID = _objects.size();
	obj->primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	_sortedObjects[obj->GetPSOName()].push_back(obj);
	_objects.push_back(move(obj));
	return _objects[_objects.size() - 1];
}


void RenderManager::UpdateMeshInstanceStartIndices()
{
	int indexStack = 0;
	for (auto& meshPair : RESOURCE->GetByType<Mesh>()) {
		shared_ptr<Mesh> mesh = static_pointer_cast<Mesh>(meshPair.second);
		if (mesh->GetInstanceCount() == 0) {
			if (_meshInstanceStartIndex.contains(mesh))
				_meshInstanceStartIndex.erase(mesh);
			continue;
		}
		_meshInstanceStartIndex[mesh] = indexStack;
		indexStack += mesh->GetInstanceCount();
	}
}

void RenderManager::RefreshMeshRenderCheckMap()
{
	vector<shared_ptr<Mesh>> unusedMesh;
	unusedMesh.reserve(_meshRenderCheckMap.size());
	for (auto& m : _meshRenderCheckMap) {
		if (m.second)
			m.second = false;
		else
			unusedMesh.push_back(m.first);
	}
	for (auto& m : unusedMesh)
		_meshRenderCheckMap.erase(m);
}

#pragma region Build_Render_Components

void RenderManager::BuildPSO(string name, D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc)
{
	ThrowIfFailed(GRAPHIC->GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(_PSOs[name].GetAddressOf())));
}

void RenderManager::BuildPSO(string name, D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc)
{
	ThrowIfFailed(GRAPHIC->GetDevice()->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(_PSOs[name].GetAddressOf())));
}

void RenderManager::BuildRootSignature()
{
	// space0 (common)
	// Structured Buffers
	CD3DX12_DESCRIPTOR_RANGE instanceTable;
	instanceTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, REGISTER_NUM_INSTANCE_SB);
	CD3DX12_DESCRIPTOR_RANGE matTable;
	matTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, REGISTER_NUM_MAT_SB);
	CD3DX12_DESCRIPTOR_RANGE lightTable;
	lightTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, REGISTER_NUM_LIGHT_SB);

	// Textures
	CD3DX12_DESCRIPTOR_RANGE cubemapTable;
	cubemapTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, REGISTER_NUM_SKYBOX_SR);
	CD3DX12_DESCRIPTOR_RANGE shadowTexTable;
	shadowTexTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, REGISTER_NUM_SHADOWMAP_SR);
	CD3DX12_DESCRIPTOR_RANGE texTable;
	texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, DEFAULT_TEXTURE_ARR_SIZE, REGISTER_NUM_TEXTURE_ARR);

	// space1 (skinned mesh)
	CD3DX12_DESCRIPTOR_RANGE boneTable;
	boneTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, REGISTER_NUM_BONE_SB, 1);

	// Slot Root Parameter
	CD3DX12_ROOT_PARAMETER slotRootParameter[ROOT_PARAMETER_COUNT];

	slotRootParameter[ROOT_PARAM_INSTCANCE_SB].InitAsDescriptorTable(1, &instanceTable);
	slotRootParameter[ROOT_PARAM_MATERIAL_SB].InitAsDescriptorTable(1, &matTable);
	slotRootParameter[ROOT_PARAM_LIGHT_SB].InitAsDescriptorTable(1, &lightTable);
	slotRootParameter[ROOT_PARAM_SKYBOX_SR].InitAsDescriptorTable(1, &cubemapTable, D3D12_SHADER_VISIBILITY_PIXEL);
	slotRootParameter[ROOT_PARAM_SHADOWMAP_SR].InitAsDescriptorTable(1, &shadowTexTable, D3D12_SHADER_VISIBILITY_PIXEL);
	slotRootParameter[ROOT_PARAM_TEXTURE_ARR].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);

	slotRootParameter[ROOT_PARAM_CLIENTINFO_C].InitAsConstants(2, REGISTER_NUM_CLIENTINFO_C);
	slotRootParameter[ROOT_PARAM_LIGHTINFO_C].InitAsConstants(1, REGISTER_NUM_LIGHTINFO_C);
	slotRootParameter[ROOT_PARAM_CAMERA_CB].InitAsConstantBufferView(REGISTER_NUM_CAMERA_CB);
	slotRootParameter[ROOT_PARAM_MESHINFO_C].InitAsConstants(1, REGISTER_NUM_MESHINFO_C);

	slotRootParameter[ROOT_PARAM_BONE_SB].InitAsDescriptorTable(1, &boneTable);

	slotRootParameter[ROOT_PARAM_PARTICLES_RW].InitAsUnorderedAccessView(REGISTER_NUM_PARTICLES_RW, 2);
	slotRootParameter[ROOT_PARAM_EMITTER_CB].InitAsConstants(sizeof(EmitterInfo) / 4, REGISTER_NUM_EMITTER_CB, 2);

	auto staticSamplers = GetStaticSamplers();

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(ROOT_PARAMETER_COUNT, slotRootParameter, (UINT)staticSamplers.size(), staticSamplers.data(),
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
}

void RenderManager::BuildSRVDescriptorHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = DESCRIPTOR_HEAP_SIZE;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	ThrowIfFailed(GRAPHIC->GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&_srvHeap)));
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
