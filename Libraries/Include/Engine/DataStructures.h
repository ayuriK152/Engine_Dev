#pragma once
#include "pch.h"

struct Vertex
{
	Vertex() {}
	Vertex(
		const Vector3& p,
		const Vector3& n,
		const Vector3& t,
		const Vector2& uv) :
		position(p),
		normal(n),
		tangent(t),
		texUV(uv) {}
	Vertex(
		float px, float py, float pz,
		float nx, float ny, float nz,
		float tx, float ty, float tz,
		float u, float v) :
		position(px, py, pz),
		normal(nx, ny, nz),
		tangent(tx, ty, tz),
		texUV(u, v) {}

	Vector3 position;
	Vector3 normal;
	Vector3 tangent;
	Vector2 texUV;
};

struct Mesh
{
	vector<Vertex> vertices;
	vector<UINT32> indices32;

	vector<UINT16>& GetIndicices16()
	{
		if (_indices16.empty())
		{
			_indices16.resize(indices32.size());
			for (size_t i = 0; i < indices32.size(); ++i)
				_indices16[i] = static_cast<UINT16>(indices32[i]);
		}

		return _indices16;
	}

private:
	vector<UINT16> _indices16;
};

struct Texture
{
	string Name;
	
	wstring Filename;

	ComPtr<ID3D12Resource> Resource = nullptr;
	ComPtr<ID3D12Resource> UploadHeap = nullptr;
};