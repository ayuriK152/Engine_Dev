#pragma once

class Geometry
{
public:
	Geometry();
	Geometry(vector<Vertex> vertices, vector<UINT16> indices);
	~Geometry() = default;

public:
	vector<Vertex>& GetVertices() { return _vertices; }
	void* GetVertexData() { return _vertices.data(); }
	UINT GetVertexCount() { return _vertices.size(); }

	vector<UINT16>& GetIndices() { return _indices; }
	void* GetIndexData() { return _indices.data(); }
	UINT GetIndexCount() { return _indices.size(); }

	void SetVertices(vector<Vertex> vertices);
	void SetIndices(vector<UINT16> indices);

	void SetWeights(UINT boneId, vector<BoneWeight>& weights);

public:
	string name;

private:
	vector<Vertex> _vertices;
	vector<UINT16> _indices;
};
