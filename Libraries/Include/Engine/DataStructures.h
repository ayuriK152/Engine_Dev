#pragma once
#include "pch.h"

struct Vertex
{
	Position position;
	Color color;
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