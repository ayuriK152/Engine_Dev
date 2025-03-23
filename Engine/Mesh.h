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

private:
	void CreateBuffer();

public:
	string name;
	UINT id;

	ComPtr<ID3D12Resource> vertexBufferGPU = nullptr;
	ComPtr<ID3D12Resource> indexBufferGPU = nullptr;

	ComPtr<ID3D12Resource> vertexBufferUploader = nullptr;
	ComPtr<ID3D12Resource> indexBufferUploader = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;

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

	void SetSkinnedMeshData(map<string, shared_ptr<Node>> nodes, map<string, shared_ptr<Bone>> bones);

private:
	vector<shared_ptr<SubMesh>> _submeshes;
	map<string, shared_ptr<Node>> _nodes;
	map<string, shared_ptr<Bone>> _bones;

	UINT _indexCount = 0;
	vector<Vertex> _vertices;
};