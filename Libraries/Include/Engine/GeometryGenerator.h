#pragma once

struct Vertex;
struct Mesh;

class GeometryGenerator
{
public:
	static Mesh CreateBox(float width, float height, float depth, UINT32 numSubdivisions);

	static void Subdivide(Mesh& meshData);
	static Vertex MidPoint(const Vertex& v0, const Vertex& v1);
};
