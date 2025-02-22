#include "pch.h"
#include "DebugHelper.h"

inline void XM_CALLCONV DrawCube(PrimitiveBatch<VertexPositionColor>* batch,
	CXMMATRIX matWorld,
	FXMVECTOR color)
{
	static const XMVECTORF32 s_verts[8] =
	{
		{ { { -1.f, -1.f, -1.f, 0.f } } },
		{ { {  1.f, -1.f, -1.f, 0.f } } },
		{ { {  1.f, -1.f,  1.f, 0.f } } },
		{ { { -1.f, -1.f,  1.f, 0.f } } },
		{ { { -1.f,  1.f, -1.f, 0.f } } },
		{ { {  1.f,  1.f, -1.f, 0.f } } },
		{ { {  1.f,  1.f,  1.f, 0.f } } },
		{ { { -1.f,  1.f,  1.f, 0.f } } }
	};

	static const WORD s_indices[] =
	{
		0, 1,
		1, 2,
		2, 3,
		3, 0,
		4, 5,
		5, 6,
		6, 7,
		7, 4,
		0, 4,
		1, 5,
		2, 6,
		3, 7
	};

	VertexPositionColor verts[8];
	for (size_t i = 0; i < 8; ++i)
	{
		XMVECTOR v = XMVector3Transform(s_verts[i], matWorld);
		XMStoreFloat3(&verts[i].position, v);
		XMStoreFloat4(&verts[i].color, color);
	}

	batch->DrawIndexed(D3D_PRIMITIVE_TOPOLOGY_LINELIST, s_indices, static_cast<UINT>(std::size(s_indices)), verts, 8);
}

void DebugHelper::Draw(PrimitiveBatch<DirectX::VertexPositionColor>* batch, const BoundingOrientedBox& obb, FXMVECTOR color /*= DirectX::Colors::Green*/)
{
	XMMATRIX matWorld = XMMatrixRotationQuaternion(XMLoadFloat4(&obb.Orientation));
	XMMATRIX matScale = XMMatrixScaling(obb.Extents.x, obb.Extents.y, obb.Extents.z);
	matWorld = XMMatrixMultiply(matScale, matWorld);
	XMVECTOR position = XMLoadFloat3(&obb.Center);
	matWorld.r[3] = XMVectorSelect(matWorld.r[3], position, g_XMSelect1110);

	DrawCube(batch, matWorld, color);
}
