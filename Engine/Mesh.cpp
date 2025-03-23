#include "pch.h"
#include "Mesh.h"

Mesh::Mesh() : Super(ResourceType::Mesh)
{

}

Mesh::Mesh(vector<shared_ptr<Geometry>> geometry) : Super(ResourceType::Mesh)
{
	for (auto& geo : geometry)
	{
		shared_ptr<SubMesh> submesh = make_shared<SubMesh>(geo);
		_submeshes.push_back(submesh);

		_indexCount += submesh->GetIndexCount();
		_vertices.insert(_vertices.begin(), submesh->GetVertices().begin(), submesh->GetVertices().end());
	}
}

Mesh::Mesh(vector<shared_ptr<SubMesh>> subMeshes) : Super(ResourceType::Mesh), _submeshes(subMeshes)
{

}

Mesh::~Mesh()
{

}

void Mesh::CreateBasicCube()
{
	CreateBasicCube(1.5f, 1.5f, 1.5f, 3);
}

void Mesh::CreateBasicCube(float width, float height, float depth, UINT numSubdivisions)
{
	shared_ptr<Geometry> geo = GeometryGenerator::CreateBox(width, height, depth, numSubdivisions);
	shared_ptr<SubMesh> subMesh = make_shared<SubMesh>(geo);
	_submeshes.push_back(subMesh);
}

void Mesh::CreateBasicSphere()
{
	return CreateBasicSphere(1.0f, 3);
}

void Mesh::CreateBasicSphere(float radius, UINT numSubdivisions)
{
	shared_ptr<Geometry> geo = GeometryGenerator::CreateGeosphere(radius, numSubdivisions);
	shared_ptr<SubMesh> subMesh = make_shared<SubMesh>(geo);
	_submeshes.push_back(subMesh);
}

void Mesh::CreateBasicQuad()
{
	shared_ptr<Geometry> geo = GeometryGenerator::CreateQuad();
	shared_ptr<SubMesh> subMesh = make_shared<SubMesh>(geo);
	_submeshes.push_back(subMesh);
}

void Mesh::SetSkinnedMeshData(map<string, shared_ptr<Node>> nodes, map<string, shared_ptr<Bone>> bones)
{
	_nodes = nodes;
	_bones = bones;
}

SubMesh::SubMesh()
{
	_material = RESOURCE->Get<Material>(L"Mat_Default");
}

SubMesh::SubMesh(shared_ptr<Geometry> geo) : _geometry(geo)
{
	_material = RESOURCE->Get<Material>(L"Mat_Default");
	CreateBuffer();
}

void SubMesh::CreateBuffer()
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
