#pragma once
#include "pch.h"

class Geometry;
class GameObject;

struct Node
{
	string name;
	UINT id;
	shared_ptr<Node> parent;
	XMFLOAT4X4 transform;
	XMFLOAT4X4 parentTransform;
};

struct Bone
{
	string name;
	UINT id;
	UINT parentId = 0;
	XMFLOAT4X4 offsetTransform;
	XMFLOAT4X4 localBindTransform;
	shared_ptr<Node> node;
	shared_ptr<GameObject> instancedObj;
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

	void AddWeight(UINT index, float weight)
	{
		for (int i = 3; i > 0; i--)
			boneIndices[i] = boneIndices[i - 1];
		boneIndices[0] = index;

		boneWeights.w = boneWeights.z;
		boneWeights.z = boneWeights.y;
		boneWeights.y = boneWeights.x;

		boneWeights.x = weight;
	}

	Vector3 Position;
	Vector3 Normal;
	Vector3 Tangent;
	Vector2 TexC;
	Vector4 boneWeights;
	INT boneIndices[4] = { -1, -1, -1, -1 };
};

struct VertexP
{
	VertexP() {};
	VertexP(const Vector3& p) : Position(p) {}
	VertexP(float px, float py, float pz) : Position(px, py, pz) {}

	Vector3 Position;
};

struct VertexPC
{
	VertexPC() {};
	VertexPC(
		const Vector3& p, 
		const ColorRGBA& c) :
		Position(p), 
		Color(c) {}
	VertexPC(
		float px, float py, float pz, 
		float cr, float cg, float cb, float ca) : 
		Position(px, py, pz), 
		Color(cr, cg, cb, ca) {}

	Vector3 Position;
	ColorRGBA Color;
};

struct VertexPT
{
	VertexPT() {};
	VertexPT(
		const Vector3& p, 
		const Vector2& t) : 
		Position(p), 
		TexC(t) {}
	VertexPT(
		float px, float py, float pz, 
		float u, float v) : 
		Position(px, py, pz), 
		TexC(u, v) {}
	Vector3 Position;
	Vector2 TexC;
};
