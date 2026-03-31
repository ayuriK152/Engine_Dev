#include "pch.h"
#include "MathHelper.h"

const float MathHelper::Infinity = FLT_MAX;
const float MathHelper::Pi = 3.14159265358979323846f;

XMFLOAT4X4 MathHelper::Identity4x4()
{
	static XMFLOAT4X4 I(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

	return I;
}

Bulb::Vector3 MathHelper::ConvertQuaternionToEuler(const Bulb::Vector4& quat)
{
	Bulb::Vector3 angles;

	// roll (x-axis rotation)
	double sinr_cosp = 2 * (quat.w * quat.x + quat.y * quat.z);
	double cosr_cosp = 1 - 2 * (quat.x * quat.x + quat.y * quat.y);
	angles.x = std::atan2(sinr_cosp, cosr_cosp);

	// pitch (y-axis rotation)
	double sinp = std::sqrt(1 + 2 * (quat.w * quat.y - quat.x * quat.z));
	double cosp = std::sqrt(1 - 2 * (quat.w * quat.y - quat.x * quat.z));
	angles.y = 2 * std::atan2(sinp, cosp) - 3.14159f / 2;

	// yaw (z-axis rotation)
	double siny_cosp = 2 * (quat.w * quat.z + quat.x * quat.y);
	double cosy_cosp = 1 - 2 * (quat.y * quat.y + quat.z * quat.z);
	angles.z = std::atan2(siny_cosp, cosy_cosp);

	return angles;
}

Bulb::Vector3 MathHelper::ConvertQuaternionToEuler(const XMVECTOR& quat)
{
	Bulb::Vector4 quatConvert;
	XMStoreFloat4(&quatConvert, quat);
	return ConvertQuaternionToEuler(quatConvert);
}

Bulb::Vector3 MathHelper::RadianToDegree(const Bulb::Vector3& radian)
{
	Bulb::Vector3 result;
	result.x = XMConvertToDegrees(radian.x);
	result.y = XMConvertToDegrees(radian.y);
	result.z = XMConvertToDegrees(radian.z);
	return result;
}

Bulb::Vector3 MathHelper::DegreeToRadian(const Bulb::Vector3& degree)
{
	Bulb::Vector3 result;
	result.x = XMConvertToRadians(degree.x);
	result.y = XMConvertToRadians(degree.y);
	result.z = XMConvertToRadians(degree.z);
	return result;
}

float MathHelper::CCW(const Bulb::Vector2& va, const Bulb::Vector2& vb)
{
	return va.x * vb.y - va.y * vb.x;
}
