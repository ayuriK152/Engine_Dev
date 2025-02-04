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

	UINT GetIndexCount() { return _geometry->GetIndexCount(); }

	void CreateBasicCube();
	void CreateBasicCube(float width, float height, float depth, UINT32 numSubdivisions);
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
	ComPtr<ID3D12Resource> vertexBufferGPU = nullptr;
	ComPtr<ID3D12Resource> indexBufferGPU = nullptr;

	ComPtr<ID3D12Resource> vertexBufferUploader = nullptr;
	ComPtr<ID3D12Resource> indexBufferUploader = nullptr;

	UINT vertexByteStride = 0;
	UINT vertexBufferByteSize = 0;
	DXGI_FORMAT indexFormat = DXGI_FORMAT_R16_UINT;
	UINT indexBufferByteSize = 0;

private:
	shared_ptr<Geometry> _geometry;
};

