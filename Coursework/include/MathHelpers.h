#pragma once

#ifndef _MATHHELP_H_
#define _MATHHELP_H_

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
#include <directxmath.h>
#include <stdexcept>
#include <algorithm>

using namespace DirectX;

inline float cleanFloat(float val) {
    return (fabs(val) < 1e-6f) ? 0.0f : val;
}
 
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


///////////////////////////////
// Quaternion to Euler
///////////////////////////////
// https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles#cite_note-2
enum RotSeq { zyx, zyz, zxy, zxz, yxz, yxy, yzx, yzy, xyz, xyx, xzy, xzx };

inline void twoaxisrot(double r11, double r12, double r21, double r31, double r32, double res[]) {
    r21 = std::clamp(r21, -1.0, 1.0);
    res[0] = atan2(r11, r12);
    res[1] = acos(r21);
    res[2] = atan2(r31, r32);
}

inline void threeaxisrot(double r11, double r12, double r21, double r31, double r32, double res[]) {
    r21 = std::clamp(r21, -1.0, 1.0);
    res[0] = atan2(r31, r32);
    res[1] = asin(r21);
    res[2] = atan2(r11, r12);
}

inline XMFLOAT3 QuaternionToEuler(const XMVECTOR& quat, RotSeq rotSeq = RotSeq::zxy)
{
    XMVECTOR normalizedQuat = XMQuaternionNormalize(quat);

    XMFLOAT4 q = {
    XMVectorGetX(normalizedQuat),
    XMVectorGetY(normalizedQuat),
    XMVectorGetZ(normalizedQuat),
    XMVectorGetW(normalizedQuat)
    };

    double res[3];
    switch (rotSeq) {
    case zyx:
        threeaxisrot(2 * (q.x * q.y + q.w * q.z),
            q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z,
            -2 * (q.x * q.z - q.w * q.y),
            2 * (q.y * q.z + q.w * q.x),
            q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z,
            res);
        break;

    case zyz:
        twoaxisrot(2 * (q.y * q.z - q.w * q.x),
            2 * (q.x * q.z + q.w * q.y),
            q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z,
            2 * (q.y * q.z + q.w * q.x),
            -2 * (q.x * q.z - q.w * q.y),
            res);
        break;

    case zxy:
        threeaxisrot(-2 * (q.x * q.y - q.w * q.z),
            q.w * q.w - q.x * q.x + q.y * q.y - q.z * q.z,
            2 * (q.y * q.z + q.w * q.x),
            -2 * (q.x * q.z - q.w * q.y),
            q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z,
            res);
        break;

    case zxz:
        twoaxisrot(2 * (q.x * q.z + q.w * q.y),
            -2 * (q.y * q.z - q.w * q.x),
            q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z,
            2 * (q.x * q.z - q.w * q.y),
            2 * (q.y * q.z + q.w * q.x),
            res);
        break;

    case yxz:
        threeaxisrot(2 * (q.x * q.z + q.w * q.y),
            q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z,
            -2 * (q.y * q.z - q.w * q.x),
            2 * (q.x * q.y + q.w * q.z),
            q.w * q.w - q.x * q.x + q.y * q.y - q.z * q.z,
            res);
        break;

    case yxy:
        twoaxisrot(2 * (q.x * q.y - q.w * q.z),
            2 * (q.y * q.z + q.w * q.x),
            q.w * q.w - q.x * q.x + q.y * q.y - q.z * q.z,
            2 * (q.x * q.y + q.w * q.z),
            -2 * (q.y * q.z - q.w * q.x),
            res);
        break;

    case yzx:
        threeaxisrot(-2 * (q.x * q.z - q.w * q.y),
            q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z,
            2 * (q.x * q.y + q.w * q.z),
            -2 * (q.y * q.z - q.w * q.x),
            q.w * q.w - q.x * q.x + q.y * q.y - q.z * q.z,
            res);
        break;

    case yzy:
        twoaxisrot(2 * (q.y * q.z + q.w * q.x),
            -2 * (q.x * q.y - q.w * q.z),
            q.w * q.w - q.x * q.x + q.y * q.y - q.z * q.z,
            2 * (q.y * q.z - q.w * q.x),
            2 * (q.x * q.y + q.w * q.z),
            res);
        break;

    case xyz:
        threeaxisrot(-2 * (q.y * q.z - q.w * q.x),
            q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z,
            2 * (q.x * q.z + q.w * q.y),
            -2 * (q.x * q.y - q.w * q.z),
            q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z,
            res);
        break;

    case xyx:
        twoaxisrot(2 * (q.x * q.y + q.w * q.z),
            -2 * (q.x * q.z - q.w * q.y),
            q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z,
            2 * (q.x * q.y - q.w * q.z),
            2 * (q.x * q.z + q.w * q.y),
            res);
        break;

    case xzy:
        threeaxisrot(2 * (q.y * q.z + q.w * q.x),
            q.w * q.w - q.x * q.x + q.y * q.y - q.z * q.z,
            -2 * (q.x * q.y - q.w * q.z),
            2 * (q.x * q.z + q.w * q.y),
            q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z,
            res);
        break;

    case xzx:
        twoaxisrot(2 * (q.x * q.z - q.w * q.y),
            2 * (q.x * q.y + q.w * q.z),
            q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z,
            2 * (q.x * q.z + q.w * q.y),
            -2 * (q.x * q.y - q.w * q.z),
            res);
        break;
    default:
        throw std::runtime_error("Unknown rotation sequence");
        break;
    }

    return XMFLOAT3(
        cleanFloat(static_cast<float>(res[0])),
        cleanFloat(static_cast<float>(res[1])),
        cleanFloat(static_cast<float>(res[2]))
        );
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

