#include "pch.h"
#include "Geometry.h"

unique_ptr<Geometry> Geometry::CreateGeometry(string geoName)
{
	unique_ptr<Geometry> geo = make_unique<Geometry>();
	geo->name = geoName;

	return geo;
}

void Geometry::AddMeshToGeo(Mesh& mesh, string meshName)
{
	_vertices.insert(_vertices.end(), begin(mesh.Vertices), end(mesh.Vertices));
	_indices16.insert(_indices16.end(), begin(mesh.GetIndicices16()), end(mesh.GetIndicices16()));

	Submesh submesh;
	submesh.indexCount = (UINT)mesh.Indices32.size();
	submesh.startIndexLocation = _submeshIndexOffset;
	submesh.baseVertexLocation = _submeshVertexOffset;

	_submeshIndexOffset += (UINT)mesh.Indices32.size();
	_submeshVertexOffset += (UINT)mesh.Vertices.size();

	this->drawArgs[meshName] = submesh;
}

void Geometry::EndCreateGeometry()
{
	const UINT vbByteSize = (UINT)_vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)_indices16.size() * sizeof(UINT16);

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &this->vertexBufferCPU));
	CopyMemory(this->vertexBufferCPU->GetBufferPointer(), _vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &this->indexBufferCPU));
	CopyMemory(this->indexBufferCPU->GetBufferPointer(), _indices16.data(), ibByteSize);

	this->vertexBufferGPU = DXUtil::CreateDefaultBuffer(GRAPHIC->GetDevice().Get(),
		GRAPHIC->GetCommandList().Get(), _vertices.data(), vbByteSize, this->vertexBufferUploader);

	this->indexBufferGPU = DXUtil::CreateDefaultBuffer(GRAPHIC->GetDevice().Get(),
		GRAPHIC->GetCommandList().Get(), _indices16.data(), ibByteSize, this->indexBufferUploader);

	this->vertexByteStride = sizeof(Vertex);
	this->vertexBufferByteSize = vbByteSize;
	this->indexFormat = DXGI_FORMAT_R16_UINT;
	this->indexBufferByteSize = ibByteSize;
}
