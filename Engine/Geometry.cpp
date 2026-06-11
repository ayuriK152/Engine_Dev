#include "pch.h"
#include "Geometry.h"

Geometry::Geometry(vector<Vertex> vertices, vector<UINT32> indices)
{
	SetVertices(vertices);
	SetIndices(indices);
}

void Geometry::SetVertices(vector<Vertex> vertices)
{
	_vertices.assign(vertices.begin(), vertices.end());
}

void Geometry::SetIndices(vector<UINT32> indices)
{
	_indices.assign(indices.begin(), indices.end());
}

void Geometry::SetWeights(UINT boneId, vector<BoneWeight>& weights)
{
	for (int i = 0; i < weights.size(); i++)
	{
		_vertices[weights[i].vertexIndex].AddWeight(boneId, weights[i].weight);
	}
}
