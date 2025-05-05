#include "pch.h"
#include "Mesh.h"

Mesh::Mesh(shared_ptr<Geometry> geometry) : Super(ResourceType::Mesh), _geometry(geometry)
{
	_material = RESOURCE->Get<Material>(L"Mat_Default");
	CreateBuffer();
}

Mesh::~Mesh()
{

}

void Mesh::SetSkinnedMeshData(map<string, shared_ptr<Node>> nodes, map<string, shared_ptr<Bone>> bones)
{
	_nodes = nodes;
	_bones = bones;
}

void Mesh::SetWeights(UINT boneId, vector<BoneWeight>& weights)
{
	_geometry->SetWeights(boneId, weights);

	const UINT vbByteSize = (UINT)_geometry->GetVertexCount() * sizeof(Vertex);

	DXUtil::UpdateBuffer(vertexBufferGPU, vertexBufferUploader, _geometry->GetVertexData(), vbByteSize);
}

void Mesh::CreateBuffer()
{
	const UINT vbByteSize = (UINT)_geometry->GetVertexCount() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)_geometry->GetIndexCount() * sizeof(UINT16);

	//ThrowIfFailed(D3DCreateBlob(vbByteSize, &this->vertexBufferCPU));
	//CopyMemory(this->vertexBufferCPU->GetBufferPointer(), _vertices.data(), vbByteSize);

	//ThrowIfFailed(D3DCreateBlob(ibByteSize, &this->indexBufferCPU));
	//CopyMemory(this->indexBufferCPU->GetBufferPointer(), _indices.data(), ibByteSize);

	vertexBufferGPU = DXUtil::CreateDefaultBuffer(GRAPHIC->GetDevice().Get(),
		GRAPHIC->GetCommandList().Get(), _geometry->GetVertexData(), vbByteSize, vertexBufferUploader);

	indexBufferGPU = DXUtil::CreateDefaultBuffer(GRAPHIC->GetDevice().Get(),
		GRAPHIC->GetCommandList().Get(), _geometry->GetIndexData(), ibByteSize, indexBufferUploader);

	{	// Vertex Buffer View Setting
		vertexBufferView.BufferLocation = vertexBufferGPU->GetGPUVirtualAddress();
		vertexBufferView.StrideInBytes = sizeof(Vertex);
		vertexBufferView.SizeInBytes = vbByteSize;
	}

	{	// Index Buffer View Setting
		indexBufferView.BufferLocation = indexBufferGPU->GetGPUVirtualAddress();
		indexBufferView.Format = DXGI_FORMAT_R16_UINT;
		indexBufferView.SizeInBytes = ibByteSize;
	}
}
