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
	vector<Vertex>& GetVertices() { return _vertices; }
	void* GetVertexData() { return _vertices.data(); }
	UINT GetVertexCount() { return _vertices.size(); }

	vector<UINT16>& GetIndices() { return _indices; }
	void* GetIndexData() { return _indices.data(); }
	UINT GetIndexCount() { return _indices.size(); }

public:
	string name;

private:
	vector<Vertex> _vertices;
	vector<UINT16> _indices;

	UINT _submeshIndexOffset = 0;
	UINT _submeshVertexOffset = 0;
};
