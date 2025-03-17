#pragma once
#include "pch.h"

class Geometry;

struct Node
{
	string name;
	UINT id;
	shared_ptr<Node> parent;
	XMFLOAT4X4 transform;
	XMFLOAT4X4 parentTransform;

	vector<UINT> submeshIndices;
};

struct Bone
{
	string name;
	shared_ptr<Node> node;
	XMFLOAT4X4 transform;
};

struct BoneWeight
{
	UINT vertexIndex;
	float weight;
};

struct Vertex
{
	Vertex() {}
	Vertex(
		const Vector3& p,
		const Vector3& n,
		const Vector3& t,
		const Vector2& uv) :
		Position(p),
		Normal(n),
		Tangent(t),
		TexC(uv) {}
	Vertex(
		float px, float py, float pz,
		float nx, float ny, float nz,
		float tx, float ty, float tz,
		float u, float v) :
		Position(px, py, pz),
		Normal(nx, ny, nz),
		Tangent(tx, ty, tz),
		TexC(u, v) {}

	void AddWeights(BoneWeight weight)
	{
		boneIndices.w = boneIndices.z;
		boneIndices.z = boneIndices.y;
		boneIndices.y = boneIndices.x;

		boneWeights.w = boneWeights.z;
		boneWeights.z = boneWeights.y;
		boneWeights.y = boneWeights.x;

		boneWeights.x = weight.weight;
	}

	Vector3 Position;
	Vector3 Normal;
	Vector3 Tangent;
	Vector2 TexC;
	Vector4 boneIndices;
	Vector4 boneWeights;
};