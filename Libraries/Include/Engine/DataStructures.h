#pragma once
#include "pch.h"

class Geometry;

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

	Vector3 Position;
	Vector3 Normal;
	Vector3 Tangent;
	Vector2 TexC;
};

struct MeshData
{
	vector<Vertex> Vertices;
	vector<UINT32> Indices32;

	vector<UINT16>& GetIndicices16()
	{
		if (_Indices16.empty())
		{
			_Indices16.resize(Indices32.size());
			for (size_t i = 0; i < Indices32.size(); ++i)
				_Indices16[i] = static_cast<UINT16>(Indices32[i]);
		}

		return _Indices16;
	}

private:
	vector<UINT16> _Indices16;
};

struct Texture
{
	string Name;
	
	wstring Filename;

	ComPtr<ID3D12Resource> Resource = nullptr;
	ComPtr<ID3D12Resource> UploadHeap = nullptr;
};