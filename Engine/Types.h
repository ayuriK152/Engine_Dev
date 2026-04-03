#pragma once
#include "DirectXMath.h"

using namespace DirectX;

namespace Bulb {
	struct BULB_API Vector2 : public XMFLOAT2
	{
		Vector2() : XMFLOAT2(0.0f, 0.0f) {}
		Vector2(float x, float y) : XMFLOAT2(x, y) {}
		Vector2(const XMFLOAT2& v) : XMFLOAT2(v.x, v.y) {}
		Vector2(const XMVECTOR& v) { XMStoreFloat2(this, v); }

		Vector2 Normalize() const {
			return Vector2(XMVector2Normalize(XMLoadFloat2(this)));
		}

		Vector2 Reflect(const Vector2& normal) const {
			return Vector2(XMVector2Reflect(XMLoadFloat2(this), XMLoadFloat2(&normal)));
		}

		float Length() const {
			return XMVectorGetX(XMVector2Length(XMLoadFloat2(this)));
		}

		Vector2 Cross(const Vector2& v) const {
			return Vector2(XMVector2Cross(XMLoadFloat2(this), XMLoadFloat2(&v)));
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

	struct BULB_API Vector3 : public XMFLOAT3
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

		float Dot(const Vector3& v) const {
			return XMVector3Dot(XMLoadFloat3(this), XMLoadFloat3(&v)).m128_f32[0];
		}

		Vector3 Lerp(const Vector3& v, float alpha) const {
			return Vector3(XMVectorLerp(XMLoadFloat3(this), XMLoadFloat3(&v), alpha));
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

	struct BULB_API Vector4 : public XMFLOAT4
	{
		Vector4() : XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f) {}
		Vector4(float x, float y, float z, float w) : XMFLOAT4(x, y, z, w) {}
		Vector4(const XMFLOAT4& v) : XMFLOAT4(v.x, v.y, v.z, v.w) {}
		Vector4(const XMVECTOR& v) { XMStoreFloat4(this, v); }

		Vector4 Normalize() const {
			return Vector4(XMQuaternionNormalize(XMLoadFloat4(this)));
		}

		Vector4 operator+(const Vector4& v) const {
			return Vector4(XMLoadFloat4(this) + XMLoadFloat4(&v));
		}

		Vector4 operator-() const {
			return Vector4(XMVectorNegate(XMLoadFloat4(this)));
		}

		Vector4 operator-(const Vector4& v) const {
			return Vector4(XMLoadFloat4(this) - XMLoadFloat4(&v));
		}

		Vector4 operator*(const Vector4& v) const {
			return Vector4(XMLoadFloat4(this) * XMLoadFloat4(&v));
		}

		Vector4 operator*(float scalar) const {
			return Vector4(XMLoadFloat4(this) * scalar);
		}

		Vector4 operator/(float scalar) const {
			assert(scalar != 0.0f);
			return Vector4(XMLoadFloat4(this) / scalar);
		}

		void operator=(const XMFLOAT4& v) {
			x = v.x;
			y = v.y;
			z = v.z;
			w = v.w;
		}
	};

	struct BULB_API Color {
		union {
			struct { float r, g, b, a; };
			XMFLOAT4 xmf4;
		};

		Color() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}
		Color(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a) {}
		Color(XMFLOAT4 v) : xmf4(v) {}
		Color(XMVECTOR v) { XMStoreFloat4(&xmf4, v); }

		Color operator+(const Color& c) const {
			return Color(XMLoadFloat4(&xmf4) + XMLoadFloat4(&c.xmf4));
		}

		Color operator-(const Color& c) const {
			return Color(XMLoadFloat4(&xmf4) - XMLoadFloat4(&c.xmf4));
		}

		Color operator*(float scalar) const {
			return Color(XMLoadFloat4(&xmf4) * scalar);
		}

		Color operator/(float scalar) const {
			assert(scalar != 0.0f);
			return Color(XMLoadFloat4(&xmf4) / scalar);
		}

		void operator=(const Color& c) {
			xmf4 = c.xmf4;
		}
	};
}
