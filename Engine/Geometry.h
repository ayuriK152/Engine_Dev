#pragma once

class BULB_API Geometry
{
public:
	Geometry() = default;
	Geometry(vector<Vertex> vertices, vector<UINT32> indices);
	~Geometry() = default;

public:
	vector<Vertex>& GetVertices() { return _vertices; }
	void* GetVertexData() { return _vertices.data(); }
	UINT GetVertexCount() { return _vertices.size(); }

	vector<UINT32>& GetIndices() { return _indices; }
	void* GetIndexData() { return _indices.data(); }
	UINT GetIndexCount() { return _indices.size(); }

	void SetVertices(vector<Vertex> vertices);
	void SetIndices(vector<UINT32> indices);

	void SetWeights(UINT boneId, vector<BoneWeight>& weights);

private:
	vector<Vertex> _vertices;
	vector<UINT32> _indices;
};
