#pragma once
#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include <directxmath.h>
#include "MathHelpers.h"

using namespace DirectX;

struct Transform
{
	inline Transform() {
		translation = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		rotation = XMQuaternionIdentity(); // not rotation
		scale = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);
	}

	inline const XMMATRIX AsMatrix() const {
		return XMMatrixScalingFromVector(scale) * XMMatrixRotationQuaternion(rotation) * XMMatrixTranslationFromVector(translation);
	}

	inline Transform operator*(const Transform& A) {
		Transform T = {};
		auto m = AsMatrix() * A.AsMatrix();
		T = m;
		return T;
	}
	__inline auto operator= (const XMMATRIX& mat) -> Transform& {
		XMMatrixDecompose(&scale, &rotation, &translation, mat);
		return *this;
	}

	inline float AsEulerX() const { return QuaternionToEuler(rotation).x; }
	inline float AsEulerY() const { return QuaternionToEuler(rotation).y; }
	inline float AsEulerZ() const { return QuaternionToEuler(rotation).z; }

	inline XMFLOAT3 AsEuler() const { return QuaternionToEuler(rotation); }

	inline void SetEulerAngles(float x, float y, float z)  { rotation = XMQuaternionRotationRollPitchYaw(x, y, z); }

	inline void SetEulerAngles(const XMVECTOR& vec) { rotation = XMQuaternionRotationRollPitchYawFromVector(vec); }

	inline void SetEulerX(float x) {
		XMFLOAT3 eulerAngles = QuaternionToEuler(rotation);
		SetEulerAngles(x, eulerAngles.y, eulerAngles.z);
	}

	inline void SetEulerY(float y) {
		XMFLOAT3 eulerAngles = QuaternionToEuler(rotation);
		SetEulerAngles(eulerAngles.x, y, eulerAngles.z);
	}

	inline void SetEulerZ(float z) {
		XMFLOAT3 eulerAngles = QuaternionToEuler(rotation);
		SetEulerAngles(eulerAngles.x, eulerAngles.y, z);
	}

	// Add a rotation quat to current transform rotation
	inline void Rotate(const XMVECTOR& deltaRot)
	{
		rotation = XMQuaternionMultiply(deltaRot, rotation);
	};
	// Add a rotation from x,y,z to current transform rotation
	inline void Rotate(const XMFLOAT3& eulerDeltaRot)
	{
		Rotate(XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&eulerDeltaRot)));
	};

	inline XMVECTOR GetForwardVector() const {
		return XMVector3Normalize(XMVector3Rotate(XMVectorSet(0.f, 0.f, 1.f, 0.f), rotation));
	}

	inline XMVECTOR const GetRightVector() const {
		return XMVector3Normalize(XMVector3Rotate(XMVectorSet(1.f, 0.f, 0.f, 0.f), rotation));
	}

	inline XMVECTOR const GetUpVector() const {
		return XMVector3Normalize(XMVector3Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), rotation));
	}

	inline void lookAt(XMVECTOR target) {
		XMVECTOR forward = XMVector3Normalize(target - translation);
		XMVECTOR up = XMVectorSet(0.f, 1.f, 0.f, 0.f);

		rotation = QuatLookAtLH(forward, up);
	}
	
	XMVECTOR translation;
	XMVECTOR rotation; // quaternion?
	XMVECTOR scale;
};

#endif