#pragma once
#include "Resource.h"

struct Vertex;

class Mesh : public Resource
{
	using Super = Resource;
public:
	Mesh();
	virtual ~Mesh();

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

public:
	vector<Vertex> Vertices;
	vector<UINT32> Indices32;

private:
	vector<UINT16> _Indices16;
};

