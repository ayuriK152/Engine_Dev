#pragma once

class DebugHelper
{
public:
	static void Draw(PrimitiveBatch<DirectX::VertexPositionColor>* batch,
		const BoundingOrientedBox& obb,
		FXMVECTOR color = DirectX::Colors::Green);
};

