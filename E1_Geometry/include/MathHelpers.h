#pragma once

#ifndef _MATHHELP_H_
#define _MATHHELP_H_

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
#include <directxmath.h>

using namespace DirectX;



XMFLOAT2 ToRadians(const XMFLOAT2& vec) {
	return {
		XMConvertToRadians(vec.x),
		XMConvertToRadians(vec.y)
	};
}

XMFLOAT3 ToRadians(const XMFLOAT3& vec) {
	return {
		XMConvertToRadians(vec.x),
		XMConvertToRadians(vec.y),
		XMConvertToRadians(vec.z)
	};
}

XMFLOAT4 ToRadians(const XMFLOAT4& vec) {
	return {
		XMConvertToRadians(vec.x),
		XMConvertToRadians(vec.y),
		XMConvertToRadians(vec.z),
		XMConvertToRadians(vec.w)
	};
}

XMFLOAT2 ToDegrees(const XMFLOAT2& vec) {
	return {
		XMConvertToRadians(vec.x),
		XMConvertToRadians(vec.y)
	};
}

XMFLOAT3 ToDegrees(const XMFLOAT3& vec) {
	return {
		XMConvertToDegrees(vec.x),
		XMConvertToDegrees(vec.y),
		XMConvertToDegrees(vec.z)
	};
}

XMFLOAT4 ToDegrees(const XMFLOAT4& vec) {
	return {
		XMConvertToDegrees(vec.x),
		XMConvertToDegrees(vec.y),
		XMConvertToDegrees(vec.z),
		XMConvertToDegrees(vec.w)
	};
}
XMFLOAT3 QuaternionToEuler(const XMVECTOR& quat)
{
	float x = XMVectorGetX(quat);
	float y = XMVectorGetY(quat);
	float z = XMVectorGetZ(quat);
	float w = XMVectorGetW(quat);

	XMFLOAT3 angles;

	// pitch (X-axis rotation)
	double sinp = 2.0 * (w * x - y * z);
	if (std::fabs(sinp) >= 1)
		angles.x = std::copysign(M_PI / 2, sinp); // clamp to 90° if out of range
	else
		angles.x = std::asin(sinp);

	// yaw (Y-axis rotation)
	double siny_cosp = 2.0 * (w * y + z * x);
	double cosy_cosp = 1.0 - 2.0 * (x * x + y * y);
	angles.y = std::atan2(siny_cosp, cosy_cosp);

	// roll (Z-axis rotation)
	double sinr_cosp = 2.0 * (w * z + x * y);
	double cosr_cosp = 1.0 - 2.0 * (y * y + z * z);
	angles.z = std::atan2(sinr_cosp, cosr_cosp);

	return angles;
}

XMVECTOR QuatLookAtLH(const XMVECTOR& forward, const XMVECTOR& up) {

	XMVECTOR adjustedForward = XMVector3Normalize(forward);

	if (XMVector3Equal(adjustedForward, up) || XMVector3Equal(adjustedForward, -up))
		return XMQuaternionIdentity();

	XMVECTOR right = XMVector3Normalize(XMVector3Cross(adjustedForward, up));
	XMVECTOR adjustedUp = XMVector3Normalize(XMVector3Cross(adjustedForward, right));

	XMMATRIX rotationMat = XMMATRIX(
		right,			// x-axis
		adjustedUp,		// y-axis
		adjustedForward,		// z-axis
		XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f)
	);

	return XMQuaternionRotationMatrix(rotationMat);
}


#endif

