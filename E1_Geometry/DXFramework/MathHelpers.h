#pragma once

#ifndef _MATHHELP_H_
#define _MATHHELP_H_

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
#include <directxmath.h>

using namespace DirectX;

inline XMFLOAT2 cleanFloat2(const XMFLOAT2& vec) {
	XMFLOAT2 returnVec = vec;
	if (abs(vec.x) < 1e-6) returnVec.x = 0.f;
	if (abs(vec.y) < 1e-6) returnVec.y = 0.f;
	return returnVec;
}

inline XMFLOAT3 cleanFloat3(const XMFLOAT3& vec) {
	XMFLOAT3 returnVec = vec;
	if (abs(vec.x) < 1e-6) returnVec.x = 0;
	if (abs(vec.y) < 1e-6) returnVec.y = 0;
	if (abs(vec.z) < 1e-6) returnVec.z = 0;
	return returnVec;
}

inline XMFLOAT4 cleanFloat4(const XMFLOAT4& vec) {
	XMFLOAT4 returnVec = vec;
	if (abs(vec.x) < 1e-6) returnVec.x = 0;
	if (abs(vec.y) < 1e-6) returnVec.y = 0;
	if (abs(vec.z) < 1e-6) returnVec.z = 0;
	if (abs(vec.w) < 1e-6) returnVec.w = 0;
	return returnVec;
}

inline XMVECTOR cleanVector3(const XMVECTOR& vec) {
	XMFLOAT3 inputVec;
	XMStoreFloat3(&inputVec, vec);
	XMFLOAT3 outputVec = cleanFloat3(inputVec);
	return XMLoadFloat3(&outputVec);
}

inline XMVECTOR cleanVector4(const XMVECTOR& vec) {
	XMFLOAT4 inputVec;
	XMStoreFloat4(&inputVec, vec);
	XMFLOAT4 outputVec = cleanFloat4(inputVec);
	return XMLoadFloat4(&outputVec);
}

inline XMFLOAT2 ToRadians(const XMFLOAT2& vec) {
	return {
		XMConvertToRadians(vec.x),
		XMConvertToRadians(vec.y)
	};
}

inline XMFLOAT3 ToRadians(const XMFLOAT3& vec) {
	return {
		XMConvertToRadians(vec.x),
		XMConvertToRadians(vec.y),
		XMConvertToRadians(vec.z)
	};
}

inline XMFLOAT4 ToRadians(const XMFLOAT4& vec) {
	return {
		XMConvertToRadians(vec.x),
		XMConvertToRadians(vec.y),
		XMConvertToRadians(vec.z),
		XMConvertToRadians(vec.w)
	};
}

inline XMFLOAT2 ToDegrees(const XMFLOAT2& vec) {
	return {
		XMConvertToRadians(vec.x),
		XMConvertToRadians(vec.y)
	};
}

inline XMFLOAT3 ToDegrees(const XMFLOAT3& vec) {
	return {
		XMConvertToDegrees(vec.x),
		XMConvertToDegrees(vec.y),
		XMConvertToDegrees(vec.z)
	};
}

inline XMFLOAT4 ToDegrees(const XMFLOAT4& vec) {
	return {
		XMConvertToDegrees(vec.x),
		XMConvertToDegrees(vec.y),
		XMConvertToDegrees(vec.z),
		XMConvertToDegrees(vec.w)
	};
}
inline XMFLOAT3 QuaternionToEuler(const XMVECTOR& quat)
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

inline XMVECTOR QuatLookAtLH(const XMVECTOR& forwardVector, const XMVECTOR& upVector) {
	//Normalize forward
	XMVECTOR forward = XMVector3Normalize(forwardVector);
	// Compute right 
	XMVECTOR right = XMVector3Normalize(XMVector3Cross(upVector, forward));
	//Recompute orthogonal UP
	XMVECTOR up = XMVector3Normalize(XMVector3Cross(forward, right));

	//Build rotation matrix
	XMMATRIX rotMat =  XMMATRIX(
		right,
		up,
		forward,
		XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
	//Convert and return as quaternion
	return XMQuaternionRotationMatrix(rotMat);
}

#endif

