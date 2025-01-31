#include "pch.h"
#include "GeometryGenerator.h"

shared_ptr<Mesh> GeometryGenerator::CreateBox(float width, float height, float depth, UINT32 numSubdivisions)
{
	shared_ptr<Mesh> mesh = make_shared<Mesh>();

	Vertex v[24];

	float w2 = 0.5f * width;
	float h2 = 0.5f * height;
	float d2 = 0.5f * depth;

	// Fill in the front face vertex data.
	v[0] = Vertex(-w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[1] = Vertex(-w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[2] = Vertex(+w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[3] = Vertex(+w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	// Fill in the back face vertex data.
	v[4] = Vertex(-w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	v[5] = Vertex(+w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[6] = Vertex(+w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[7] = Vertex(-w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// Fill in the top face vertex data.
	v[8] = Vertex(-w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[9] = Vertex(-w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[10] = Vertex(+w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[11] = Vertex(+w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	// Fill in the bottom face vertex data.
	v[12] = Vertex(-w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	v[13] = Vertex(+w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[14] = Vertex(+w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[15] = Vertex(-w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// Fill in the left face vertex data.
	v[16] = Vertex(-w2, -h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[17] = Vertex(-w2, +h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[18] = Vertex(-w2, +h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[19] = Vertex(-w2, -h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	// Fill in the right face vertex data.
	v[20] = Vertex(+w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	v[21] = Vertex(+w2, +h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	v[22] = Vertex(+w2, +h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	v[23] = Vertex(+w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

	mesh->Vertices.assign(&v[0], &v[24]);

	//
	// Create the indices.
	//

	UINT32 i[36];

	// Fill in the front face index data
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;

	// Fill in the back face index data
	i[6] = 4; i[7] = 5; i[8] = 6;
	i[9] = 4; i[10] = 6; i[11] = 7;

	// Fill in the top face index data
	i[12] = 8; i[13] = 9; i[14] = 10;
	i[15] = 8; i[16] = 10; i[17] = 11;

	// Fill in the bottom face index data
	i[18] = 12; i[19] = 13; i[20] = 14;
	i[21] = 12; i[22] = 14; i[23] = 15;

	// Fill in the left face index data
	i[24] = 16; i[25] = 17; i[26] = 18;
	i[27] = 16; i[28] = 18; i[29] = 19;

	// Fill in the right face index data
	i[30] = 20; i[31] = 21; i[32] = 22;
	i[33] = 20; i[34] = 22; i[35] = 23;

	mesh->Indices32.assign(&i[0], &i[36]);

	// Put a cap on the number of subdivisions.
	numSubdivisions = std::min<UINT32>(numSubdivisions, 6u);

	for (UINT32 i = 0; i < numSubdivisions; ++i)
		Subdivide(mesh);

	return mesh;
}

shared_ptr<Mesh> GeometryGenerator::CreateGeosphere(float radius, UINT32 numSubdivisions)
{
	shared_ptr<Mesh> mesh = make_shared<Mesh>();

	// Put a cap on the number of subdivisions.
	numSubdivisions = std::min<UINT32>(numSubdivisions, 6u);

	// Approximate a sphere by tessellating an icosahedron.

	const float X = 0.525731f;
	const float Z = 0.850651f;

	XMFLOAT3 pos[12] =
	{
		XMFLOAT3(-X, 0.0f, Z),  XMFLOAT3(X, 0.0f, Z),
		XMFLOAT3(-X, 0.0f, -Z), XMFLOAT3(X, 0.0f, -Z),
		XMFLOAT3(0.0f, Z, X),   XMFLOAT3(0.0f, Z, -X),
		XMFLOAT3(0.0f, -Z, X),  XMFLOAT3(0.0f, -Z, -X),
		XMFLOAT3(Z, X, 0.0f),   XMFLOAT3(-Z, X, 0.0f),
		XMFLOAT3(Z, -X, 0.0f),  XMFLOAT3(-Z, -X, 0.0f)
	};

	UINT32 k[60] =
	{
		1,4,0,  4,9,0,  4,5,9,  8,5,4,  1,8,4,
		1,10,8, 10,3,8, 8,3,5,  3,2,5,  3,7,2,
		3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
		10,1,6, 11,0,9, 2,11,9, 5,2,9,  11,2,7
	};

	mesh->Vertices.resize(12);
	mesh->Indices32.assign(&k[0], &k[60]);

	for (UINT32 i = 0; i < 12; ++i)
		mesh->Vertices[i].Position = pos[i];

	for (UINT32 i = 0; i < numSubdivisions; ++i)
		Subdivide(mesh);

	// Project vertices onto sphere and scale.
	for (UINT32 i = 0; i < mesh->Vertices.size(); ++i)
	{
		// Project onto unit sphere.
		XMVECTOR n = XMVector3Normalize(XMLoadFloat3(&mesh->Vertices[i].Position));

		// Project onto sphere.
		XMVECTOR p = radius * n;

		XMStoreFloat3(&mesh->Vertices[i].Position, p);
		XMStoreFloat3(&mesh->Vertices[i].Normal, n);

		// Derive texture coordinates from spherical coordinates.
		float theta = atan2f(mesh->Vertices[i].Position.z, mesh->Vertices[i].Position.x);

		// Put in [0, 2pi].
		if (theta < 0.0f)
			theta += XM_2PI;

		float phi = acosf(mesh->Vertices[i].Position.y / radius);

		mesh->Vertices[i].TexC.x = theta / XM_2PI;
		mesh->Vertices[i].TexC.y = phi / XM_PI;

		// Partial derivative of P with respect to theta
		mesh->Vertices[i].Tangent.x = -radius * sinf(phi) * sinf(theta);
		mesh->Vertices[i].Tangent.y = 0.0f;
		mesh->Vertices[i].Tangent.z = +radius * sinf(phi) * cosf(theta);

		XMVECTOR T = XMLoadFloat3(&mesh->Vertices[i].Tangent);
		XMStoreFloat3(&mesh->Vertices[i].Tangent, XMVector3Normalize(T));
	}

	return mesh;
}

void GeometryGenerator::Subdivide(shared_ptr<Mesh> mesh)
{
	// Save a copy of the input geometry.
	auto indicesCopy = mesh->Indices32;
	auto verticesCopy = mesh->Vertices;

	mesh->Vertices.resize(0);
	mesh->Indices32.resize(0);

	UINT32 numTris = (UINT32)indicesCopy.size() / 3;
	for (UINT32 i = 0; i < numTris; ++i)
	{
		Vertex v0 = verticesCopy[indicesCopy[i * 3 + 0]];
		Vertex v1 = verticesCopy[indicesCopy[i * 3 + 1]];
		Vertex v2 = verticesCopy[indicesCopy[i * 3 + 2]];

		//
		// Generate the midpoints.
		//

		Vertex m0 = MidPoint(v0, v1);
		Vertex m1 = MidPoint(v1, v2);
		Vertex m2 = MidPoint(v0, v2);

		//
		// Add new geometry.
		//

		mesh->Vertices.push_back(v0); // 0
		mesh->Vertices.push_back(v1); // 1
		mesh->Vertices.push_back(v2); // 2
		mesh->Vertices.push_back(m0); // 3
		mesh->Vertices.push_back(m1); // 4
		mesh->Vertices.push_back(m2); // 5

		mesh->Indices32.push_back(i * 6 + 0);
		mesh->Indices32.push_back(i * 6 + 3);
		mesh->Indices32.push_back(i * 6 + 5);

		mesh->Indices32.push_back(i * 6 + 3);
		mesh->Indices32.push_back(i * 6 + 4);
		mesh->Indices32.push_back(i * 6 + 5);

		mesh->Indices32.push_back(i * 6 + 5);
		mesh->Indices32.push_back(i * 6 + 4);
		mesh->Indices32.push_back(i * 6 + 2);

		mesh->Indices32.push_back(i * 6 + 3);
		mesh->Indices32.push_back(i * 6 + 1);
		mesh->Indices32.push_back(i * 6 + 4);
	}
}

Vertex GeometryGenerator::MidPoint(const Vertex& v0, const Vertex& v1)
{
	XMVECTOR p0 = XMLoadFloat3(&v0.Position);
	XMVECTOR p1 = XMLoadFloat3(&v1.Position);

	XMVECTOR n0 = XMLoadFloat3(&v0.Normal);
	XMVECTOR n1 = XMLoadFloat3(&v1.Normal);

	XMVECTOR tan0 = XMLoadFloat3(&v0.Tangent);
	XMVECTOR tan1 = XMLoadFloat3(&v1.Tangent);

	XMVECTOR tex0 = XMLoadFloat2(&v0.TexC);
	XMVECTOR tex1 = XMLoadFloat2(&v1.TexC);

	// Compute the midpoints of all the attributes.  Vectors need to be normalized
	// since linear interpolating can make them not unit length.  
	XMVECTOR pos = 0.5f * (p0 + p1);
	XMVECTOR normal = XMVector3Normalize(0.5f * (n0 + n1));
	XMVECTOR tangent = XMVector3Normalize(0.5f * (tan0 + tan1));
	XMVECTOR tex = 0.5f * (tex0 + tex1);

	Vertex v;
	XMStoreFloat3(&v.Position, pos);
	XMStoreFloat3(&v.Normal, normal);
	XMStoreFloat3(&v.Tangent, tangent);
	XMStoreFloat2(&v.TexC, tex);

	return v;
}
