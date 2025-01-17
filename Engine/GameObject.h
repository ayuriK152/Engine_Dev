#pragma once

#pragma region 전방선언
class Mesh;
#pragma endregion


class GameObject
{
public:
	GameObject();
	~GameObject();

public:
	XMFLOAT4X4 world;

	Mesh* mesh;
	D3D12_PRIMITIVE_TOPOLOGY primitiveType;

	UINT objCBIndex;
	UINT indexCount;
	UINT startIndexLocation;
	int baseVertexLocation;

	int numFramesDirty;
};
