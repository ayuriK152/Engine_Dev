#pragma once
#include "Resource.h"

#pragma region 전방선언
struct Vertex;
#pragma endregion

class SubMesh
{
public:
	SubMesh();
	SubMesh(shared_ptr<Geometry> geo);
	~SubMesh() = default;

public:
	UINT GetIndexCount() { return _geometry->GetIndexCount(); }

	vector<Vertex>& GetVertices() { return _geometry->GetVertices(); }
	void* GetVertexData() { return _geometry->GetVertices().data(); }
	UINT GetVertexCount() { return _geometry->GetVertices().size(); }

	void SetMaterial(shared_ptr<Material> mat) { _material = mat; }
	const shared_ptr<Material> GetMaterial() { return _material; }

	D3D12_VERTEX_BUFFER_VIEW VertexBufferView()const
	{
		D3D12_VERTEX_BUFFER_VIEW vbv;
		vbv.BufferLocation = vertexBufferGPU->GetGPUVirtualAddress();
		vbv.StrideInBytes = vertexByteStride;
		vbv.SizeInBytes = vertexBufferByteSize;

		return vbv;
	}

	D3D12_INDEX_BUFFER_VIEW IndexBufferView()const
	{
		D3D12_INDEX_BUFFER_VIEW ibv;
		ibv.BufferLocation = indexBufferGPU->GetGPUVirtualAddress();
		ibv.Format = indexFormat;
		ibv.SizeInBytes = indexBufferByteSize;

		return ibv;
	}

	void CreateBuffer();

public:
	ComPtr<ID3D12Resource> vertexBufferGPU = nullptr;
	ComPtr<ID3D12Resource> indexBufferGPU = nullptr;

	ComPtr<ID3D12Resource> vertexBufferUploader = nullptr;
	ComPtr<ID3D12Resource> indexBufferUploader = nullptr;

	UINT vertexByteStride = 0;
	UINT vertexBufferByteSize = 0;
	DXGI_FORMAT indexFormat = DXGI_FORMAT_R16_UINT;
	UINT indexBufferByteSize = 0;

private:
	shared_ptr<Geometry> _geometry;
	shared_ptr<Material> _material;
};

class Mesh : public Resource
{
	using Super = Resource;
public:
	Mesh();
	Mesh(vector<shared_ptr<Geometry>> geometry);
	Mesh(vector<shared_ptr<SubMesh>> subMeshes);
	virtual ~Mesh();

public:
	UINT GetIndexCount() { return _indexCount; }

	vector<Vertex>& GetVertices() { return _vertices; }
	void* GetVertexData() { return _vertices.data(); }
	UINT GetVertexCount() { return _vertices.size(); }

	void CreateBasicCube();
	void CreateBasicCube(float width, float height, float depth, UINT numSubdivisions);
	void CreateBasicSphere();
	void CreateBasicSphere(float radius, UINT numSubdivisions);
	void CreateBasicQuad();

	const vector<shared_ptr<SubMesh>> GetSubmeshes() { return _submeshes; }

private:
	vector<shared_ptr<SubMesh>> _submeshes;

	UINT _indexCount = 0;
	vector<Vertex> _vertices;
};