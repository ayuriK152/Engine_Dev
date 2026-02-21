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

// dirty flag를 넣던가 해서 최적화 작업 필요
void UIManager::Update()
{
	XMMATRIX viewProj = XMLoadFloat4x4(&Camera::GetViewMatrix()) * XMLoadFloat4x4(&Camera::GetProjMatrix());

	_uiConstants.clear();
	for (auto& ui : _uiArray) {
		float width = ui->_size.x * (0.5f - ui->_pivot.x);
		float height = ui->_size.y * (0.5f - ui->_pivot.y);
		Vector4 clipPos(XMVector3Transform(XMLoadFloat3(&ui->_localPosition), viewProj));
		Vector2 ndc(clipPos.x / clipPos.w, clipPos.y / clipPos.w);

		UIInstanceConstants constants;
		constants.CenterPos = { ((ndc.x) * 0.5f * 1920.0f) + width, ((-ndc.y) * 0.5f * 1080.0f) + height };	// 해상도 원하는대로 설정하도록
		constants.Color = ui->_color;
		constants.Depth = 0.5f;
		constants.TextureIndex = ui->_textureSrvHeapIndex;
		constants.Size = ui->_size;

		_uiConstants.push_back(constants);
	}

	_uploadBuffer->CopyData(_uiConstants.data(), _uiArray.size());
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
	++_uiCount;
	_uiArray.push_back(ui);
}

void UIManager::DeleteUI(const shared_ptr<UIElement>& ui)
{
	for (int i = 0; i < _uiCount; ++i) {
		if (_uiArray[i] == ui) {
			--_uiCount;
			_uiArray.erase(_uiArray.begin() + i);
		}
	}
}

void UIManager::CreateTestUI()
{
	++_uiCount;
	UIInstanceConstants uiConstant;
	uiConstant.CenterPos = { 0.0f, 0.0f };
	uiConstant.Color = { 1.0f, 1.0f, 1.0f, 1.0f };
	uiConstant.Depth = 0.0f;
	uiConstant.Size = { 30.0f, 30.0f };
	uiConstant.TextureIndex = RESOURCE->Get<Texture>(L"LockOnMarker")->GetSRVHeapIndex();
	_uiConstants.push_back(uiConstant);

	_uploadBuffer->CopyData(_uiConstants.data(), 1);
}
