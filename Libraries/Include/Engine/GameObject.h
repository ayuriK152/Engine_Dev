#pragma once

#pragma region 전방선언
class Geometry;
class Material;
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
	Material* material;
	D3D12_PRIMITIVE_TOPOLOGY primitiveType;

	string meshName;

	UINT objCBIndex;
	UINT indexCount;
	UINT startIndexLocation;
	int baseVertexLocation;

	int numFramesDirty;
};
