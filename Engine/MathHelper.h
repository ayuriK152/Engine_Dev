#pragma once


class MathHelper
{
public:
	static const float Infinity;
	static const float Pi;

public:
	static XMFLOAT4X4 Identity4x4();

	static Vector3 ConvertQuaternionToEuler(const Vector4& quat);
	static Vector3 ConvertQuaternionToEuler(const XMVECTOR& quat);

	static Vector2 VectorSubtract(const Vector2& va, const Vector2& vb);
	static Vector3 VectorSubtract(const Vector3& va, const Vector3& vb);
	static Vector2 VectorAddition(const Vector2& va, const Vector2& vb);
	static Vector3 VectorAddition(const Vector3& va, const Vector3& vb);

	static float CCW(const Vector2& va, const Vector2& vb);
};

