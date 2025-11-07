#pragma once

#include <directxmath.h>
#include "MathHelpers.h"

using namespace DirectX;

struct Transform
{
	Transform() {
		translation = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		rotation = XMQuaternionIdentity(); // not rotation
		scale = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);
	}

	const XMMATRIX AsMatrix() const {
		return XMMatrixScalingFromVector(scale) * XMMatrixRotationQuaternion(rotation) * XMMatrixTranslationFromVector(translation);
	}

	Transform operator*(const Transform& A) {
		Transform T = {};
		auto m = AsMatrix() * A.AsMatrix();
		T = m;
		return T;
	}
	__inline auto operator= (const XMMATRIX& mat) -> Transform& {
		XMMatrixDecompose(&scale, &rotation, &translation, mat);
		return *this;
	}

	float AsEulerX() const { return QuaternionToEuler(rotation).x; }
	float AsEulerY() const { return QuaternionToEuler(rotation).y; }
	float AsEulerZ() const { return QuaternionToEuler(rotation).z; }

	XMFLOAT3 AsEuler() const { return QuaternionToEuler(rotation); }

	void SetEulerAngles(float x, float y, float z)  { rotation = XMQuaternionRotationRollPitchYaw(x, y, z); }

	void SetEulerAngles(const XMVECTOR& vec) { rotation = XMQuaternionRotationRollPitchYawFromVector(vec); }

	void SetEulerX(float x) {
		XMFLOAT3 eulerAngles = QuaternionToEuler(rotation);
		SetEulerAngles(x, eulerAngles.y, eulerAngles.z);
	}

	void SetEulerY(float y) {
		XMFLOAT3 eulerAngles = QuaternionToEuler(rotation);
		SetEulerAngles(eulerAngles.x, y, eulerAngles.z);
	}

	void SetEulerZ(float z) {
		XMFLOAT3 eulerAngles = QuaternionToEuler(rotation);
		SetEulerAngles(eulerAngles.x, eulerAngles.y, z);
	}

	// Add a rotation quat to current transform rotation
	void Rotate(const XMVECTOR& deltaRot)
	{
		rotation = XMQuaternionMultiply(deltaRot, rotation);
	};
	// Add a rotation from x,y,z to current transform rotation
	void Rotate(const XMFLOAT3& eulerDeltaRot)
	{
		Rotate(XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&eulerDeltaRot));
	};

	XMVECTOR GetForwardVector() const {
		return XMVector3Normalize(XMVector3Rotate(XMVectorSet(0.f, 0.f, 1.f, 0.f), rotation));
	}

	XMVECTOR const GetRightVector() const {
		return XMVector3Normalize(XMVector3Rotate(XMVectorSet(1.f, 0.f, 0.f, 0.f), rotation));
	}

	XMVECTOR const GetUpVector() const {
		return XMVector3Normalize(XMVector3Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), rotation));
	}

	void lookAt(XMVECTOR target) {
		XMVECTOR forward = XMVector3Normalize(target - translation);
		XMVECTOR up = XMVectorSet(0.f, 1.f, 0.f, 0.f);

		rotation = QuatLookAtLH(forward, up);
	}
	
	XMVECTOR translation;
	XMVECTOR rotation; // quaternion?
	XMVECTOR scale;
};