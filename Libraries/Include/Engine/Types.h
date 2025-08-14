#pragma once
#include "DirectXMath.h"

using namespace DirectX;

using Vector4		= XMFLOAT4;

struct Vector2 : public XMFLOAT2
{
	Vector2() : XMFLOAT2(0.0f, 0.0f) {}
	Vector2(float x, float y) : XMFLOAT2(x, y) {}
	Vector2(const XMFLOAT2& v) : XMFLOAT2(v.x, v.y) {}
	Vector2(const XMVECTOR& v) { XMStoreFloat2(this, v); }

	Vector2 Normalize() const {
		return Vector2(XMVector3Normalize(XMLoadFloat2(this)));
	}

	Vector2 Reflect(const Vector2& normal) const {
		return Vector2(XMVector3Reflect(XMLoadFloat2(this), XMLoadFloat2(&normal)));
	}

	float Length() const {
		return XMVectorGetX(XMVector3Length(XMLoadFloat2(this)));
	}

	Vector2 Cross(const Vector2& v) const {
		return Vector2(XMVector3Cross(XMLoadFloat2(this), XMLoadFloat2(&v)));
	}

	Vector2 operator+(const Vector2& v) const {
		return Vector2(XMLoadFloat2(this) + XMLoadFloat2(&v));
	}

	Vector2 operator-() const {
		return Vector2(XMVectorNegate(XMLoadFloat2(this)));
	}

	Vector2 operator-(const Vector2& v) const {
		return Vector2(XMLoadFloat2(this) - XMLoadFloat2(&v));
	}

	Vector2 operator*(const Vector2& v) const {
		return Vector2(XMLoadFloat2(this) * XMLoadFloat2(&v));
	}

	Vector2 operator*(float scalar) const {
		return Vector2(XMLoadFloat2(this) * scalar);
	}

	Vector2 operator/(float scalar) const {
		assert(scalar != 0.0f);
		return Vector2(XMLoadFloat2(this) / scalar);
	}

	void operator=(const XMFLOAT2& v) {
		x = v.x;
		y = v.y;
	}
};

struct Vector3 : public XMFLOAT3
{
	Vector3() : XMFLOAT3(0.0f, 0.0f, 0.0f) {}
	Vector3(float x, float y, float z) : XMFLOAT3(x, y, z) {}
	Vector3(const XMFLOAT3& v) : XMFLOAT3(v.x, v.y, v.z) {}
	Vector3(const XMVECTOR& v) { XMStoreFloat3(this, v); }

	Vector3 Normalize() const {
		return Vector3(XMVector3Normalize(XMLoadFloat3(this)));
	}

	Vector3 Reflect(const Vector3& normal) const {
		return Vector3(XMVector3Reflect(XMLoadFloat3(this), XMLoadFloat3(&normal)));
	}

	float Length() const {
		return XMVectorGetX(XMVector3Length(XMLoadFloat3(this)));
	}

	Vector3 Cross(const Vector3& v) const {
		return Vector3(XMVector3Cross(XMLoadFloat3(this), XMLoadFloat3(&v)));
	}

	Vector3 operator+(const Vector3& v) const {
		return Vector3(XMLoadFloat3(this) + XMLoadFloat3(&v));
	}

	Vector3 operator-() const {
		return Vector3(XMVectorNegate(XMLoadFloat3(this)));
	}

	Vector3 operator-(const Vector3& v) const {
		return Vector3(XMLoadFloat3(this) - XMLoadFloat3(&v));
	}

	Vector3 operator*(const Vector3& v) const {
		return Vector3(XMLoadFloat3(this) * XMLoadFloat3(&v));
	}

	Vector3 operator*(float scalar) const {
		return Vector3(XMLoadFloat3(this) * scalar);
	}

	Vector3 operator/(float scalar) const {
		assert(scalar != 0.0f);
		return Vector3(XMLoadFloat3(this) / scalar);
	}

	void operator=(const XMFLOAT3& v) {
		x = v.x;
		y = v.y;
		z = v.z;
	}
};

using ColorRGBA		= XMFLOAT4;
using Color			= ColorRGBA;
using ColorRGB		= XMFLOAT3;