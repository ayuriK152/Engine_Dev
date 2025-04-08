#include "pch.h"
#include "RenderManager.h"

void RenderManager::Init()
{
	BuildRootSignature();
	BuildInputLayout();
	BuildSRVDescriptorHeap();
	RESOURCE->CreateDefaultResources();

	{
		auto opaqueSolid = CreatePSODesc(_inputLayout, L"standardVS", L"opaquePS");
		auto opaqueSkinned = CreatePSODesc(_skinnedInputLayout, L"skinnedVS", L"opaquePS");
		auto opaqueWireframe = opaqueSolid;
		opaqueWireframe.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
		BuildPSO(PSO_OPAQUE_SOLID, opaqueSolid);
		BuildPSO(PSO_OPAQUE_SKINNED, opaqueSkinned);
		BuildPSO(PSO_WIREFRAME, opaqueWireframe);
		SetDefaultPSO();
	}

	_materialCB = make_unique<UploadBuffer<MaterialConstants>>(20, true);
	_cameraCB = make_unique<UploadBuffer<CameraConstants>>(1, true);
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
	ID3D12DescriptorHeap* descriptorHeaps[] = { _srvHeap.Get() };
	GRAPHIC->GetCommandList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	GRAPHIC->GetCommandList()->SetGraphicsRootSignature(_rootSignature.Get());

	//auto passCB = GRAPHIC->GetCurrFrameResource()->passCB->GetResource();
	//GRAPHIC->GetCommandList()->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_LIGHT_CB, passCB->GetGPUVirtualAddress());
	auto lightCB = GRAPHIC->GetCurrFrameResource()->lightCB->GetResource();
	GRAPHIC->GetCommandList()->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_LIGHT_CB, lightCB->GetGPUVirtualAddress());
	GRAPHIC->GetCommandList()->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_CAMERA_CB, _cameraCB->GetResource()->GetGPUVirtualAddress());

	if (_isPSOFixed)
	{
		GRAPHIC->GetCommandList()->SetPipelineState(_currPSO.Get());
		for (int i = 0; i < _objects.size(); i++)
			_objects[i]->Render();
	}
	else
	{
		map<string, vector<shared_ptr<GameObject>>> sortedObjects;
		for (int i = 0; i < _objects.size(); i++)
		{
			if (!sortedObjects.contains(_objects[i]->psoName))
				sortedObjects.insert({ _objects[i]->psoName, { } });
			sortedObjects[_objects[i]->psoName].push_back(_objects[i]);
		}
		for (auto& p : sortedObjects)
		{
			GRAPHIC->GetCommandList()->SetPipelineState(_PSOs[p.first].Get());
			for (int i = 0; i < p.second.size(); i++)
				p.second[i]->Render();
		}
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

shared_ptr<GameObject> RenderManager::AddGameObject(shared_ptr<GameObject> obj)
{
	obj->objCBIndex = _objects.size();
	obj->primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	_objects.push_back(move(obj));
	return _objects[_objects.size() - 1];
}

#pragma endregion


#pragma region Build_Render_Components

void RenderManager::BuildPSO(string name, D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc)
{
	ThrowIfFailed(GRAPHIC->GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(_PSOs[name].GetAddressOf())));
}

void RenderManager::BuildRootSignature()
{
	CD3DX12_DESCRIPTOR_RANGE texTable;
	texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, ROOT_PARAMETER_TEXTURE_SR);
	CD3DX12_DESCRIPTOR_RANGE boneTable;
	boneTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, ROOT_PARAMETER_BONE_SB);

	CD3DX12_ROOT_PARAMETER slotRootParameter[6];

	slotRootParameter[ROOT_PARAMETER_TEXTURE_SR].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);	// TextureSR
	slotRootParameter[ROOT_PARAMETER_BONE_SB].InitAsDescriptorTable(1, &boneTable, D3D12_SHADER_VISIBILITY_VERTEX);	// BoneSB
	slotRootParameter[ROOT_PARAMETER_LIGHT_CB].InitAsConstantBufferView(0);		// LightCB
	slotRootParameter[ROOT_PARAMETER_OBJECT_CB].InitAsConstantBufferView(1);	// ObjectCB
	slotRootParameter[ROOT_PARAMETER_MATERIAL_CB].InitAsConstantBufferView(2);	// MaterialCB
	slotRootParameter[ROOT_PARAMETER_CAMERA_CB].InitAsConstantBufferView(3);	// CameraCB

	auto staticSamplers = GetStaticSamplers();

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(6, slotRootParameter, (UINT)staticSamplers.size(), staticSamplers.data(),
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
	_inputLayout =
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
}

void RenderManager::BuildSRVDescriptorHeap()
{
	// Descriptor의 갯수를 50으로 고정 제한중인데 유동적으로 가변으로 바꾸는 작업 고려
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 50;
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


std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> RenderManager::GetStaticSamplers()
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
