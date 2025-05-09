#pragma once
#include "Resource.h"

#pragma region ���漱��
struct Vertex;
#pragma endregion

class Mesh : public Resource
{
	using Super = Resource;
public:
	Mesh(shared_ptr<Geometry> geometry);
	virtual ~Mesh();

// refactoring
public:
	UINT GetIndexCount() { return _geometry->GetIndexCount(); }

	vector<Vertex>& GetVertices() { return _geometry->GetVertices(); }
	void* GetVertexData() { return _geometry->GetVertices().data(); }
	UINT GetVertexCount() { return _geometry->GetVertices().size(); }

	void SetMaterial(shared_ptr<Material> mat) {
		_material = mat;
	}
	const shared_ptr<Material> GetMaterial() { return _material; }

	void SetWeights(UINT boneId, vector<BoneWeight>& weights);

// refactoring
private:
	void CreateBuffer();

// refactoring
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
	UINT _indexCount = 0;

	//============================
	// refactoring
	shared_ptr<Geometry> _geometry;
	shared_ptr<Material> _material;
};