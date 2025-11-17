#pragma once
#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include "MathHelpers.h"
#include "ImGuiHelpers.h"
#include "SerializationHelpers.h"
#include "Nlohmann/json.hpp"

using namespace DirectX;

class Transform
{
private:
	XMVECTOR translation;
	XMVECTOR rotation; // quaternion?
	XMVECTOR scale;
	XMMATRIX localMatrix;
	XMMATRIX globalMatrix;
	Transform* parentTransform = nullptr;
public:

	inline Transform() {
		translation = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		rotation = XMQuaternionIdentity(); // not rotation
		scale = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);
		computeGlobalMatrix(true);
	}

	inline void setParent(Transform* parent) { parentTransform = parent; }

	const XMMATRIX& getLocalMatrix() { return localMatrix; }
	const XMMATRIX& getGlobalMatrix() { return globalMatrix; }

	inline const XMVECTOR& GetRotationQuat() const { return rotation; }
	inline const XMVECTOR& GetTranslationVector() const { return translation; }
	inline const XMVECTOR& GetScaleVector() const { return scale; }

	inline void computeLocalMatrix() {
		localMatrix = XMMatrixScalingFromVector(scale)* XMMatrixRotationQuaternion(rotation)* XMMatrixTranslationFromVector(translation);
	}

	inline void computeGlobalMatrix(bool computeLocal = true) {
		if (computeLocal) { computeLocalMatrix(); }
		if (parentTransform != nullptr) {
			globalMatrix = localMatrix * parentTransform->getGlobalMatrix();
		} else {
			globalMatrix = localMatrix;
		}
	}

	__inline auto operator= (const XMMATRIX& mat) -> Transform& {
		XMMatrixDecompose(&scale, &rotation, &translation, mat);
		return *this;
	}

	inline const XMMATRIX& getGlobalMatrix() { return globalMatrix; }

	inline XMFLOAT3 GetLocalTranslation() const {
		XMFLOAT3 translationVec;
		XMStoreFloat3(&translationVec, translation);
		return translationVec;
	}

	inline XMFLOAT3 GetLocalScale() const {
		XMFLOAT3 scaleVec;
		XMStoreFloat3(&scaleVec, scale);
		return scaleVec;
	}

	inline float GetEulerX() const { return QuaternionToEuler(rotation).x; }
	inline float GetEulerY() const { return QuaternionToEuler(rotation).y; }
	inline float GetEulerZ() const { return QuaternionToEuler(rotation).z; }

	inline const XMFLOAT3 GetEuler() const { return QuaternionToEuler(rotation); }

	inline void translate(float x, float y, float z) {
		translation = XMVectorAdd(translation, XMVectorSet(x, y, z, 0.0f));
		computeGlobalMatrix(true);
	}

	inline void translate(const XMVECTOR& vec) {
		translation = XMVectorAdd(translation, vec);
		computeGlobalMatrix(true);
	}

	inline void translate(const XMFLOAT3& vec) {
		translation = XMVectorAdd(translation, XMLoadFloat3(&vec));
		computeGlobalMatrix(true);
	}

	inline void setPosition(float x, float y, float z) 
	{ 
		translation = XMVectorSet(x, y, z, 0.f); 
		computeGlobalMatrix(true);
	}
	inline void setScale(float x, float y, float z) 
	{ 
		scale = XMVectorSet(x, y, z, 0.f); 
		computeGlobalMatrix(true);
	}

	inline void SetPosition(const XMFLOAT3& pos) 
	{ 
		translation = XMLoadFloat3(&pos); 
		computeGlobalMatrix(true);
	}
	inline void SetScale(const XMFLOAT3& s) 
	{ 
		scale = XMLoadFloat3(&s); 
		computeGlobalMatrix(true);
	}

	inline void SetPositionVector(const XMVECTOR& position) 
	{ 
		translation = position; 
		computeGlobalMatrix(true);
	}
	inline void SetScaleVector(const XMVECTOR& scale) 
	{ 
		this->scale = scale; 
		computeGlobalMatrix(true);
	}
	inline void setRotationQuat(const XMVECTOR& rotation) 
	{ 
		this->rotation = rotation; 
		computeGlobalMatrix(true);
	}

	inline void SetEulerAngles(float x, float y, float z) 
	{ 
		rotation = XMQuaternionRotationRollPitchYaw(x, y, z); 
		computeGlobalMatrix(true);
	}

	inline void SetEulerAngles(const XMVECTOR& vec) 
	{ 
		rotation = XMQuaternionRotationRollPitchYawFromVector(vec); 
		computeGlobalMatrix(true);
	}

	inline void SetEulerX(float x) {
		XMFLOAT3 eulerAngles = QuaternionToEuler(rotation);
		SetEulerAngles(x, eulerAngles.y, eulerAngles.z);
		computeGlobalMatrix(true);
	}

	inline void SetEulerY(float y) {
		XMFLOAT3 eulerAngles = QuaternionToEuler(rotation);
		SetEulerAngles(eulerAngles.x, y, eulerAngles.z);
		computeGlobalMatrix(true);
	}

	inline void SetEulerZ(float z) {
		XMFLOAT3 eulerAngles = QuaternionToEuler(rotation);
		SetEulerAngles(eulerAngles.x, eulerAngles.y, z);
		computeGlobalMatrix(true);
	}

	// Add a rotation quat to current transform rotation
	inline void Rotate(const XMVECTOR& deltaRot)
	{
		rotation = XMQuaternionMultiply(deltaRot, rotation);
		computeGlobalMatrix(true);
	};
	// Add a rotation from x,y,z to current transform rotation
	inline void Rotate(const XMFLOAT3& eulerDeltaRot)
	{
		Rotate(XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&eulerDeltaRot)));
		computeGlobalMatrix(true);
	};

	inline XMVECTOR GetLocalForwardVector() const {
		return XMVector3Normalize(XMVector3Rotate(XMVectorSet(0.f, 0.f, 1.f, 0.f), rotation));
	}

	inline XMVECTOR GetLocalRightVector() const {
		return XMVector3Normalize(XMVector3Rotate(XMVectorSet(1.f, 0.f, 0.f, 0.f), rotation));
	}

	inline XMVECTOR GetLocalUpVector() const {
		return XMVector3Normalize(XMVector3Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), rotation));
	}

	XMVECTOR GetWorldForward() const { return XMVector3Normalize(XMVector3TransformNormal({ 0,0,1,0 }, globalMatrix)); }
	XMVECTOR GetWorldRight()   const { return XMVector3Normalize(XMVector3TransformNormal({ 1,0,0,0 }, globalMatrix)); }
	XMVECTOR GetWorldUp()      const { return XMVector3Normalize(XMVector3TransformNormal({ 0,1,0,0 }, globalMatrix)); }

	inline void lookAt(XMVECTOR target) {
		XMVECTOR worldPos = XMVector3TransformCoord(translation, parentTransform ? parentTransform->globalMatrix : XMMatrixIdentity());
		XMVECTOR forward = XMVector3Normalize(target - worldPos);
		XMVECTOR up = XMVectorSet(0.f, 1.f, 0.f, 0.f);

		XMVECTOR worldRot = QuatLookAtLH(forward, up);

		if (parentTransform)
			rotation = XMQuaternionMultiply(worldRot, XMQuaternionInverse(parentTransform->GetRotationQuat()));
		else
			rotation = worldRot;

		computeGlobalMatrix(true);
	}

	inline bool ImGuiRender(const std::string& label, int menuNum = 0, bool editTranslation = true, bool editRotation = true, bool editScale = true) {
		bool transformUpdated = false;
		if (editTranslation && ImGuiDragXMVECTOR3((label + " Translation: ## " + std::to_string(menuNum)).c_str(), translation)) { transformUpdated = true; }
		if (editRotation && ImGuiQuatEulerSlider3Degrees((label + " Rotation: ## " + std::to_string(menuNum)).c_str(), rotation)) { transformUpdated = true; }
		if (editScale && ImGuiDragXMVECTOR3((label + " Scale: ## " + std::to_string(menuNum)).c_str(), scale)) { transformUpdated = true; }
		return transformUpdated;
	}
};
namespace nlohmann {
	template<>
	struct adl_serializer<Transform> {
		static void to_json(json& j, const Transform& t) {
			j["translation"] = t.GetTranslationVector();
			j["rotation"] = t.GetRotationQuat();
			j["scale"] = t.GetScaleVector();       // XMVECTOR serializer used
		}

		static void from_json(const json& j, Transform& t) {
			t.SetPositionVector(j.at("translation").get<XMVECTOR>());
			t.setRotationQuat(j.at("rotation").get<XMVECTOR>());
			t.SetScaleVector(j.at("scale").get<XMVECTOR>());
		}
	};
}

#endif