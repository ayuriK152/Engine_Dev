#include "pch.h"
#include "Geometry.h"

unique_ptr<Geometry> Geometry::CreateGeometry(Mesh& mesh, string geoName, string meshName)
{
	const UINT vbByteSize = (UINT)mesh.vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)mesh.GetIndicices16().size() * sizeof(UINT16);

	Submesh submesh;
	submesh.indexCount = (UINT)mesh.indices32.size();
	submesh.startIndexLocation = 0;
	submesh.baseVertexLocation = 0;

	unique_ptr<Geometry> geo = make_unique<Geometry>();
	geo->name = geoName;

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->vertexBufferCPU));
	CopyMemory(geo->vertexBufferCPU->GetBufferPointer(), mesh.vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->indexBufferCPU));
	CopyMemory(geo->indexBufferCPU->GetBufferPointer(), mesh.GetIndicices16().data(), ibByteSize);



	geo->vertexBufferGPU = DXUtil::CreateDefaultBuffer(GRAPHIC->GetDevice().Get(),
		GRAPHIC->GetCommandList().Get(), mesh.vertices.data(), vbByteSize, geo->vertexBufferUploader);

	geo->indexBufferGPU = DXUtil::CreateDefaultBuffer(GRAPHIC->GetDevice().Get(),
		GRAPHIC->GetCommandList().Get(), mesh.GetIndicices16().data(), ibByteSize, geo->indexBufferUploader);

	geo->vertexByteStride = sizeof(Vertex);
	geo->vertexBufferByteSize = vbByteSize;
	geo->indexFormat = DXGI_FORMAT_R16_UINT;
	geo->indexBufferByteSize = ibByteSize;

	geo->drawArgs[meshName] = submesh;

	return geo;
}
