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
};

