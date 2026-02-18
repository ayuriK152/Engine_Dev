#include "pch.h"
#include "UIManager.h"

UIManager::~UIManager()
{

}

void UIManager::Init()
{
	_quadMesh = RESOURCE->Get<Mesh>(DEFAULT_MESH_QUAD);
	_quadMesh->CreateBuffer();

	CreateBuffer();
}

void UIManager::Render(ID3D12GraphicsCommandList* cmdList)
{
	if (_uiCount <= 0) return;

	cmdList->IASetVertexBuffers(0, 1, &_quadMesh->vertexBufferView);
	cmdList->IASetIndexBuffer(&_quadMesh->indexBufferView);
	cmdList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	cmdList->DrawIndexedInstanced(_quadMesh->GetIndexCount(), _uiCount, 0, 0, 0);
}

void UIManager::CreateBuffer()
{
	// Create Buffer
	if (_uploadBuffer != nullptr)
		_uploadBuffer.release();
	_uploadBuffer = make_unique<UploadBuffer<UIInstanceConstants>>(DEFAULT_UI_COUNT, false);

	// Create SRV
	_uploadBufferSrvIndex = RENDER->GetAndIncreaseSRVHeapIndex();
	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(RENDER->GetCommonSRVHeap()->GetCPUDescriptorHandleForHeapStart());
	hDescriptor.Offset(_uploadBufferSrvIndex, GRAPHIC->GetCBVSRVDescriptorSize());

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = DEFAULT_UI_COUNT;
	srvDesc.Buffer.StructureByteStride = sizeof(UIInstanceConstants);
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	GRAPHIC->GetDevice()->CreateShaderResourceView(_uploadBuffer->GetResource(), &srvDesc, hDescriptor);
}

void UIManager::AddUI(const shared_ptr<UIElement>& ui)
{
	_uiArray.push_back(ui);
}

void UIManager::CreateTestUI()
{
	++_uiCount;
	UIInstanceConstants uiConstant;
	uiConstant.CenterPos = { 0.0f, 0.0f };
	uiConstant.Color = { 1.0f, 1.0f, 1.0f, 0.3f };
	uiConstant.Depth = 0.0f;
	uiConstant.Size = { 250.0f, 90.0f };
	uiConstant.TextureIndex = RESOURCE->Get<Texture>(L"Tex_Default")->GetSRVHeapIndex();
	_uiConstants.push_back(uiConstant);

	_uploadBuffer->CopyData(_uiConstants.data(), 1);
}
