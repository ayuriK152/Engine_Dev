#include "pch.h"
#include "Terrain.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

unique_ptr<UploadBuffer<InstanceConstants>> Terrain::_terrainInstanceSB = nullptr;

int Terrain::_terrainCount = 0;

Terrain::Terrain() : Component(ComponentType::Terrain)
{

}

Terrain::~Terrain()
{
#ifdef PRINT_DEBUG_CONSOLE_LOG
	cout << "Released - Terrain:" << _id << "\n";
#endif
}

void Terrain::Init()
{
	if (_terrainInstanceSB == nullptr) {
		_terrainInstanceSB = make_unique<UploadBuffer<InstanceConstants>>(DEFAULT_TERRAIN_COUNT, false);
		BuildBuffer();
	}

	_terrainId = _terrainCount++;
	UpdateConstant();

	// TEST
	SetTestHeightSample();

	// 설정 객체 생성
	JPH::HeightFieldShapeSettings settings;
	settings.mOffset = Vec3(0, 0, 0); // 지형의 시작점 오프셋
	settings.mScale = Vec3(1.0f, _heightFactor, 1.0f); // 각 샘플 사이의 거리 및 높이 배율
	settings.mSampleCount = _sampleCount;
	settings.mHeightSamples.resize(_sampleCount * _sampleCount);
	for (int y = 0; y < _sampleCount; ++y) {
		for (int x = 0; x < _sampleCount; ++x) {
			settings.mHeightSamples[y * _sampleCount + x] = _heightSamples[y * _sampleCount + x];
		}
	}

	_shapeResult = settings.Create();

	JPH::BodyCreationSettings bodySettings(_shapeResult.Get(),
		JPH::RVec3(0, 0, 0),
		JPH::Quat(0, 0, 0, 1),
		EMotionType::Static,
		Layers::NON_MOVING);

	bodySettings.mUserData = reinterpret_cast<JPH::uint64>(_gameObject.lock().get());

	JPH::BodyInterface& bodyInterface = PHYSICS->GetPhysicsSystem()->GetBodyInterface();
	_bodyID = bodyInterface.CreateAndAddBody(bodySettings, JPH::EActivation::Activate);
}

void Terrain::PreUpdate()
{

}

void Terrain::Update()
{
	if (GetGameObject()->GetFramesDirty() > 0) {
		UpdateConstant();
	}
}

void Terrain::Render(ID3D12GraphicsCommandList* cmdList, UINT renderState)
{
	if (_terrainMesh == nullptr) return;

	CD3DX12_GPU_DESCRIPTOR_HANDLE terrain(RENDER->GetCommonSRVHeap()->GetGPUDescriptorHandleForHeapStart());
	terrain.Offset(_instanceSrvIdx, GRAPHIC->GetCBVSRVDescriptorSize());

	cmdList->SetGraphicsRootDescriptorTable(ROOT_PARAM_TERRAIN_SB, terrain);

	cmdList->IASetVertexBuffers(0, 1, &_terrainMesh->vertexBufferView);
	cmdList->IASetIndexBuffer(&_terrainMesh->indexBufferView);
	cmdList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	cmdList->SetGraphicsRoot32BitConstant(ROOT_PARAM_MESHINFO_C, 0, 0);
	cmdList->SetGraphicsRoot32BitConstant(ROOT_PARAM_MESHINFO_C, MESHTYPE_TERRAIN, 1);
	cmdList->SetGraphicsRoot32BitConstant(ROOT_PARAM_TERRAININFO_C, _terrainId, 0);
	cmdList->SetGraphicsRoot32BitConstant(ROOT_PARAM_TERRAININFO_C, _heightMapTextureIndex, 1);
	cmdList->SetGraphicsRoot32BitConstant(ROOT_PARAM_TERRAININFO_C, _heightFactor, 2);
	
	cmdList->DrawIndexedInstanced(_terrainMesh->GetIndexCount(), 1, 0, 0, 0);
}

void Terrain::OnDestroy()
{
#ifdef PRINT_DEBUG_CONSOLE_LOG
	cout << "OnDestroy - Terrain:" << _id << "\n";
#endif

	JPH::BodyInterface& bodyInterface = PHYSICS->GetPhysicsSystem()->GetBodyInterface();
	bodyInterface.RemoveBody(_bodyID);
	bodyInterface.DestroyBody(_bodyID);
	PHYSICS->DeleteRigidbody(static_pointer_cast<Rigidbody>(shared_from_this()));
}

void Terrain::BuildBuffer()
{
	_instanceSrvIdx = RENDER->GetAndIncreaseSRVHeapIndex();
	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(RENDER->GetCommonSRVHeap()->GetCPUDescriptorHandleForHeapStart());
	hDescriptor.Offset(_instanceSrvIdx, GRAPHIC->GetCBVSRVDescriptorSize());

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = DEFAULT_TERRAIN_COUNT;
	srvDesc.Buffer.StructureByteStride = sizeof(InstanceConstants);
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	GRAPHIC->GetDevice()->CreateShaderResourceView(_terrainInstanceSB->GetResource(), &srvDesc, hDescriptor);
}

void Terrain::ReleaseBuffer()
{

}

void Terrain::SetTestHeightSample()
{
	SetHeightMap(RESOURCE->Get<Texture>(L"..\\Resources\\Textures\\HeightMap.png"));

	_terrainMesh = make_shared<Mesh>(GeometryGenerator::CreateTerrain(_sampleCount));
	_terrainMesh->CreateBuffer();
}

void Terrain::LoadXML(Bulb::XMLElement compElem)
{

}

void Terrain::SaveXML(Bulb::XMLElement compElem)
{
	compElem.SetAttribute("ComponentType", "Terrain");
}

ComponentSnapshot Terrain::CaptureSnapshot()
{
	ComponentSnapshot snapshot;

	snapshot.id = _id;
	snapshot.componentType = "Terrain";

	return snapshot;
}

void Terrain::RestoreSnapshot(ComponentSnapshot snapshot)
{

}

void Terrain::SetHeightMap(shared_ptr<Texture> texture)
{
	_heightMapTextureIndex = texture->GetSRVHeapIndex();
	_heightMapTexturePath = texture->GetPath();
	
	_heightFactor = 10.0f;

	int height, width, channels;
	unsigned char* samples = stbi_load(_heightMapTexturePath.c_str(), &width, &height, &channels, 1);

	_sampleCount = max(height, width);

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			_heightSamples.push_back(samples[y * height + x] / 255.0f);
		}
	}
}

void Terrain::UpdateConstant()
{
	InstanceConstants terrainConstants;

	terrainConstants.MaterialIndex = 0;
	XMMATRIX world = XMLoadFloat4x4(&GetTransform()->GetWorldMatrix());
	XMStoreFloat4x4(&terrainConstants.World, XMMatrixTranspose(world));
	XMStoreFloat4x4(&terrainConstants.WorldInv, XMMatrixTranspose(XMMatrixInverse(nullptr, world)));

	_terrainInstanceSB->CopyData(_terrainId, terrainConstants);
}
