#pragma once

class Submesh
{
	UINT indexCount = 0;
	UINT startIndexLocation = 0;
	INT baseVertexLocation = 0;

	DirectX::BoundingBox bounds;
};

