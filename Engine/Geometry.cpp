#include "pch.h"
#include "Geometry.h"

void Geometry::SetVertices(vector<Vertex> vertices)
{
	_vertices.assign(vertices.begin(), vertices.end());
}

void Geometry::SetIndices(vector<UINT16> indices)
{
	_indices.assign(indices.begin(), indices.end());
}
