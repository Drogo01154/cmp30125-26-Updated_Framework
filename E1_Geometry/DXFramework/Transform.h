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

	inline const XMVECTOR& getRotationQuat() const { return rotation; }
	inline const XMVECTOR& getTranslationVector() const { return translation; }
	inline const XMVECTOR& getScaleVector() const { return scale; }

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

	inline XMFLOAT3 getLocalTranslation() const {
		XMFLOAT3 translationVec;
		XMStoreFloat3(&translationVec, translation);
		return translationVec;
	}

	inline XMFLOAT3 getLocalScale() const {
		XMFLOAT3 scaleVec;
		XMStoreFloat3(&scaleVec, scale);
		return scaleVec;
	}

	inline float getEulerX() const { return QuaternionToEuler(rotation).x; }
	inline float getEulerY() const { return QuaternionToEuler(rotation).y; }
	inline float getEulerZ() const { return QuaternionToEuler(rotation).z; }

	inline const XMFLOAT3 getEuler() const { return QuaternionToEuler(rotation); }

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

	inline void setPosition(const XMFLOAT3& pos) 
	{ 
		translation = XMLoadFloat3(&pos); 
		computeGlobalMatrix(true);
	}
	inline void setScale(const XMFLOAT3& s) 
	{ 
		scale = XMLoadFloat3(&s); 
		computeGlobalMatrix(true);
	}

	inline void setPositionVector(const XMVECTOR& position) 
	{ 
		translation = position; 
		computeGlobalMatrix(true);
	}
	inline void setScaleVector(const XMVECTOR& scale) 
	{ 
		this->scale = scale; 
		computeGlobalMatrix(true);
	}
	inline void setRotationQuat(const XMVECTOR& rotation) 
	{ 
		this->rotation = rotation; 
		computeGlobalMatrix(true);
	}

	inline void setEulerAngles(float x, float y, float z) 
	{ 
		rotation = XMQuaternionRotationRollPitchYaw(x, y, z); 
		computeGlobalMatrix(true);
	}

	inline void setEulerAngles(const XMVECTOR& vec) 
	{ 
		rotation = XMQuaternionRotationRollPitchYawFromVector(vec); 
		computeGlobalMatrix(true);
	}

	inline void setEulerX(float x) {
		XMFLOAT3 eulerAngles = QuaternionToEuler(rotation);
		setEulerAngles(x, eulerAngles.y, eulerAngles.z);
		computeGlobalMatrix(true);
	}

	inline void SetEulerY(float y) {
		XMFLOAT3 eulerAngles = QuaternionToEuler(rotation);
		setEulerAngles(eulerAngles.x, y, eulerAngles.z);
		computeGlobalMatrix(true);
	}

	inline void setEulerZ(float z) {
		XMFLOAT3 eulerAngles = QuaternionToEuler(rotation);
		setEulerAngles(eulerAngles.x, eulerAngles.y, z);
		computeGlobalMatrix(true);
	}

	// Add a rotation quat to current transform rotation
	inline void rotate(const XMVECTOR& deltaRot)
	{
		rotation = XMQuaternionMultiply(deltaRot, rotation);
		computeGlobalMatrix(true);
	};
	// Add a rotation from x,y,z to current transform rotation
	inline void rotate(const XMFLOAT3& eulerDeltaRot)
	{
		rotate(XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&eulerDeltaRot)));
		computeGlobalMatrix(true);
	};

	inline XMVECTOR getLocalForwardVector() const {
		return XMVector3Normalize(XMVector3Rotate(XMVectorSet(0.f, 0.f, 1.f, 0.f), rotation));
	}

	inline XMVECTOR getLocalRightVector() const {
		return XMVector3Normalize(XMVector3Rotate(XMVectorSet(1.f, 0.f, 0.f, 0.f), rotation));
	}

	inline XMVECTOR getLocalUpVector() const {
		return XMVector3Normalize(XMVector3Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), rotation));
	}

	XMVECTOR getWorldForward() const { return XMVector3Normalize(XMVector3TransformNormal({ 0,0,1,0 }, globalMatrix)); }
	XMVECTOR getWorldRight()   const { return XMVector3Normalize(XMVector3TransformNormal({ 1,0,0,0 }, globalMatrix)); }
	XMVECTOR getWorldUp()      const { return XMVector3Normalize(XMVector3TransformNormal({ 0,1,0,0 }, globalMatrix)); }

	inline void lookAt(XMVECTOR target) {
		XMVECTOR worldPos = XMVector3TransformCoord(translation, parentTransform ? parentTransform->globalMatrix : XMMatrixIdentity());
		XMVECTOR forward = XMVector3Normalize(target - worldPos);
		XMVECTOR up = XMVectorSet(0.f, 1.f, 0.f, 0.f);

		XMVECTOR worldRot = QuatLookAtLH(forward, up);

		if (parentTransform)
			rotation = XMQuaternionMultiply(worldRot, XMQuaternionInverse(parentTransform->getRotationQuat()));
		else
			rotation = worldRot;

		computeGlobalMatrix(true);
	}

	inline bool imGuiRender(const std::string& label, int menuNum = 0, bool editTranslation = true, bool editRotation = true, bool editScale = true) {
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
			j["translation"] = t.getTranslationVector();
			j["rotation"] = t.getRotationQuat();
			j["scale"] = t.getScaleVector();       // XMVECTOR serializer used
		}

		static void from_json(const json& j, Transform& t) {
			t.setPositionVector(j.at("translation").get<XMVECTOR>());
			t.setRotationQuat(j.at("rotation").get<XMVECTOR>());
			t.setScaleVector(j.at("scale").get<XMVECTOR>());
		}
	};
}

#endif