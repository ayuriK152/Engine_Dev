#pragma once
#include "Resource.h"

#pragma region 전방선언
struct Vertex;
#pragma endregion

class Mesh : public Resource
{
	using Super = Resource;
public:
	Mesh();
	virtual ~Mesh();

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

	UINT GetIndexCount() { return _geometry->GetIndexCount(); }

	void CreateBasicSphere();
	void CreateBasicSphere(float radius, UINT32 numSubdivisions);

	D3D12_VERTEX_BUFFER_VIEW VertexBufferView()const
	{
		D3D12_VERTEX_BUFFER_VIEW vbv;
		vbv.BufferLocation = vertexBufferGPU->GetGPUVirtualAddress();
		vbv.StrideInBytes = vertexByteStride;
		vbv.SizeInBytes = vertexBufferByteSize;

		return vbv;
	}

	D3D12_INDEX_BUFFER_VIEW IndexBufferView()const
	{
		D3D12_INDEX_BUFFER_VIEW ibv;
		ibv.BufferLocation = indexBufferGPU->GetGPUVirtualAddress();
		ibv.Format = indexFormat;
		ibv.SizeInBytes = indexBufferByteSize;

		return ibv;
	}

	void CreateBuffer();

public:
	vector<Vertex> vertices;
	vector<UINT32> indices32;

	ComPtr<ID3D12Resource> vertexBufferGPU = nullptr;
	ComPtr<ID3D12Resource> indexBufferGPU = nullptr;

	ComPtr<ID3D12Resource> vertexBufferUploader = nullptr;
	ComPtr<ID3D12Resource> indexBufferUploader = nullptr;

	UINT vertexByteStride = 0;
	UINT vertexBufferByteSize = 0;
	DXGI_FORMAT indexFormat = DXGI_FORMAT_R16_UINT;
	UINT indexBufferByteSize = 0;

private:
	vector<UINT16> _indices16;
	shared_ptr<Geometry> _geometry;
};

