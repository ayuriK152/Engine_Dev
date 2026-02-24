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
	for (auto& ui : _elements) {
		ui->Update();
	}

	XMMATRIX viewProj = XMLoadFloat4x4(&Camera::GetViewMatrix()) * XMLoadFloat4x4(&Camera::GetProjMatrix());

	_uiConstants.clear();
	for (auto& ui : _panels) {
		UIInstanceConstants constants;
		shared_ptr<UITransform> transform = ui->GetTransform();
		Vector3 position = transform->GetPosition();
		Vector2 pivot = transform->GetPivot();

		float width = ui->_size.x * (0.5f - pivot.x);
		float height = ui->_size.y * (pivot.y - 0.5f);
		if (transform->IsDynamicPosition()) {
			Vector4 clipPos(XMVector3Transform(XMLoadFloat3(&position), viewProj));
			Vector2 ndc(clipPos.x / clipPos.w, clipPos.y / clipPos.w);

			constants.CenterPos = { (ndc.x * 0.5f * _uiResolution.x) + width, (-ndc.y * 0.5f * _uiResolution.y) + height };	// 해상도 원하는대로 설정하도록
		}

		else {
			constants.CenterPos = { position.x + width, position.y + height };
		}

		constants.Color = ui->_color;
		constants.Depth = transform->GetDepth();
		constants.TextureIndex = ui->_textureSrvHeapIndex;
		constants.Size = ui->_size;

		_uiConstants.push_back(constants);
	}

	_uploadBuffer->CopyData(_uiConstants.data(), _elements.size());
}

void UIManager::Render(ID3D12GraphicsCommandList* cmdList)
{
	if (_panelCount <= 0) return;

	cmdList->IASetVertexBuffers(0, 1, &_quadMesh->vertexBufferView);
	cmdList->IASetIndexBuffer(&_quadMesh->indexBufferView);
	cmdList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	cmdList->DrawIndexedInstanced(_quadMesh->GetIndexCount(), _panels.size(), 0, 0, 0);
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
	++_elementCount;
	_elements.push_back(ui);
	if (ui->_type == UIType::Panel) {
		++_panelCount;
		_panels.push_back(static_pointer_cast<UIPanel>(ui));
	}
}

void UIManager::DeleteUI(const shared_ptr<UIElement>& ui)
{
	for (int i = 0; i < _elementCount; ++i) {
		if (_elements[i] == ui) {
			--_elementCount;
			_elements.erase(_elements.begin() + i);
		}
	}

	if (ui->_type == UIType::Panel) {
		for (int i = 0; i < _panels.size(); ++i) {
			if (_panels[i] == ui) {
				_panels.erase(_panels.begin() + i);
			}
		}
	}
}
