#pragma once

struct Vertex;
struct Mesh;

class GeometryGenerator
{
	DECLARE_SINGLE(GeometryGenerator);
public:
	Mesh CreateBox(float width, float height, float depth, UINT32 numSubdivisions);

	void Subdivide(Mesh& meshData);
	Vertex MidPoint(const Vertex& v0, const Vertex& v1);
};
