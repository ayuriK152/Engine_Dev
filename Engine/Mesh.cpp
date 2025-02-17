#include "pch.h"
#include "Mesh.h"

Mesh::Mesh() : Super(ResourceType::Mesh)
{

}

Mesh::~Mesh()
{

}

void Mesh::CreateBasicCube()
{
	CreateBasicCube(1.5f, 1.5f, 1.5f, 3);
}

void Mesh::CreateBasicCube(float width, float height, float depth, UINT32 numSubdivisions)
{
	_geometry = GeometryGenerator::CreateBox(width, height, depth, numSubdivisions);
	CreateBuffer();
}

void Mesh::CreateBasicSphere()
{
	return CreateBasicSphere(1.0f, 0);
}

void Mesh::CreateBasicSphere(float radius, UINT32 numSubdivisions)
{
	_geometry = GeometryGenerator::CreateGeosphere(radius, numSubdivisions);
	CreateBuffer();
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

	vertexByteStride = sizeof(Vertex);
	vertexBufferByteSize = vbByteSize;
	indexFormat = DXGI_FORMAT_R16_UINT;
	indexBufferByteSize = ibByteSize;
}
