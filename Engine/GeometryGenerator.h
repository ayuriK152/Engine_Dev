#pragma once

struct Vertex;
class Mesh;

class GeometryGenerator
{
public:
	static shared_ptr<Mesh> CreateBox(float width, float height, float depth, UINT32 numSubdivisions);
	static shared_ptr<Mesh> CreateGeosphere(float radius, UINT32 numSubdivisions);

	static void Subdivide(shared_ptr<Mesh> mesh);
	static Vertex MidPoint(const Vertex& v0, const Vertex& v1);
};
