#pragma once

class Submesh
{
public:
	UINT indexCount = 0;
	UINT startIndexLocation = 0;
	INT baseVertexLocation = 0;

	BoundingBox bounds;
};

class Geometry
{
public:
	//D3D12_VERTEX_BUFFER_VIEW VertexBufferView()const
	//{
	//	D3D12_VERTEX_BUFFER_VIEW vbv;
	//	vbv.BufferLocation = vertexBufferGPU->GetGPUVirtualAddress();
	//	vbv.StrideInBytes = vertexByteStride;
	//	vbv.SizeInBytes = vertexBufferByteSize;

	//	return vbv;
	//}

	//D3D12_INDEX_BUFFER_VIEW IndexBufferView()const
	//{
	//	D3D12_INDEX_BUFFER_VIEW ibv;
	//	ibv.BufferLocation = indexBufferGPU->GetGPUVirtualAddress();
	//	ibv.Format = indexFormat;
	//	ibv.SizeInBytes = indexBufferByteSize;

	//	return ibv;
	//}

	//void DisposeUploaders()
	//{
	//	vertexBufferUploader = nullptr;
	//	indexBufferUploader = nullptr;
	//}

	vector<Vertex>& GetVertices() { return _vertices; }
	void* GetVertexData() { return _vertices.data(); }
	UINT GetVertexCount() { return _vertices.size(); }

	vector<UINT16>& GetIndices() { return _indices; }
	void* GetIndexData() { return _indices.data(); }
	UINT GetIndexCount() { return _indices.size(); }

	//static unique_ptr<Geometry> CreateGeometry(string geoName);
	//void AddMeshToGeo(shared_ptr<Mesh> mesh, string meshName);
	//void EndCreateGeometry();

public:
	string name;

	// 왜씀?(진짜 모름)
	//ComPtr<ID3DBlob> vertexBufferCPU = nullptr;
	//ComPtr<ID3DBlob> indexBufferCPU = nullptr;

	//ComPtr<ID3D12Resource> vertexBufferGPU = nullptr;
	//ComPtr<ID3D12Resource> indexBufferGPU = nullptr;

	//ComPtr<ID3D12Resource> vertexBufferUploader = nullptr;
	//ComPtr<ID3D12Resource> indexBufferUploader = nullptr;

	//UINT vertexByteStride = 0;
	//UINT vertexBufferByteSize = 0;
	//DXGI_FORMAT indexFormat = DXGI_FORMAT_R16_UINT;
	//UINT indexBufferByteSize = 0;

	//unordered_map<string, Submesh> drawArgs;

private:
	vector<Vertex> _vertices;
	vector<UINT16> _indices;

	UINT _submeshIndexOffset = 0;
	UINT _submeshVertexOffset = 0;
};
