#include "pch.h"
#include "GeometryGenerator.h"

Mesh GeometryGenerator::CreateBox(float width, float height, float depth, UINT32 numSubdivisions)
{
	Mesh meshData;

	Vertex vertices[8] =
	{
		Vertex({ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::White) }),
		Vertex({ XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::White) }),
		Vertex({ XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::White) }),
		Vertex({ XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::White) }),
		Vertex({ XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::White) }),
		Vertex({ XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::White) }),
		Vertex({ XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::White) }),
		Vertex({ XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::White) })
	};

	UINT16 indices[36] =
	{
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
	};

	meshData.vertices.assign(&vertices[0], &vertices[8]);
	meshData.indices32.assign(&indices[0], &indices[36]);

	numSubdivisions = min<UINT32>(numSubdivisions, 6u);

	for (UINT32 i = 0; i < numSubdivisions; ++i)
		Subdivide(meshData);

	return meshData;
}

void GeometryGenerator::Subdivide(Mesh& meshData)
{
	// Save a copy of the input geometry.
	Mesh inputCopy = meshData;

	meshData.vertices.resize(0);
	meshData.indices32.resize(0);

	UINT32 numTris = (UINT32)inputCopy.indices32.size() / 3;
	for (UINT32 i = 0; i < numTris; ++i)
	{
		Vertex v0 = inputCopy.vertices[inputCopy.indices32[i * 3 + 0]];
		Vertex v1 = inputCopy.vertices[inputCopy.indices32[i * 3 + 1]];
		Vertex v2 = inputCopy.vertices[inputCopy.indices32[i * 3 + 2]];

		//
		// Generate the midpoints.
		//

		Vertex m0 = MidPoint(v0, v1);
		Vertex m1 = MidPoint(v1, v2);
		Vertex m2 = MidPoint(v0, v2);

		//
		// Add new geometry.
		//

		meshData.vertices.push_back(v0); // 0
		meshData.vertices.push_back(v1); // 1
		meshData.vertices.push_back(v2); // 2
		meshData.vertices.push_back(m0); // 3
		meshData.vertices.push_back(m1); // 4
		meshData.vertices.push_back(m2); // 5

		meshData.indices32.push_back(i * 6 + 0);
		meshData.indices32.push_back(i * 6 + 3);
		meshData.indices32.push_back(i * 6 + 5);

		meshData.indices32.push_back(i * 6 + 3);
		meshData.indices32.push_back(i * 6 + 4);
		meshData.indices32.push_back(i * 6 + 5);

		meshData.indices32.push_back(i * 6 + 5);
		meshData.indices32.push_back(i * 6 + 4);
		meshData.indices32.push_back(i * 6 + 2);

		meshData.indices32.push_back(i * 6 + 3);
		meshData.indices32.push_back(i * 6 + 1);
		meshData.indices32.push_back(i * 6 + 4);
	}
}

Vertex GeometryGenerator::MidPoint(const Vertex& v0, const Vertex& v1)
{
	XMVECTOR p0 = XMLoadFloat3(&(XMFLOAT3)v0.position);
	XMVECTOR p1 = XMLoadFloat3(&(XMFLOAT3)v1.position);

	XMVECTOR c0 = XMLoadFloat4(&(XMFLOAT4)v0.color);
	XMVECTOR c1 = XMLoadFloat4(&(XMFLOAT4)v1.color);

	XMVECTOR pos = 0.5f * (p0 + p1);
	XMVECTOR color = 0.5f * (c0 + c1);

	Vertex v;
	XMStoreFloat3(&v.position, pos);
	XMStoreFloat4(&v.color, color);

	return v;
}
