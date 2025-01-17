#pragma once

#pragma region 전방선언
class Geometry;
#pragma endregion


class GameObject
{
public:
	GameObject();
	~GameObject();

	void Render();

public:
	XMFLOAT4X4 world;

	Geometry* geometry;
	D3D12_PRIMITIVE_TOPOLOGY primitiveType;

	UINT objCBIndex;
	UINT indexCount;
	UINT startIndexLocation;
	int baseVertexLocation;

	int numFramesDirty;
};
