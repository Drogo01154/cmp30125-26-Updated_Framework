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
	XMVECTOR rotation;
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
	inline Transform* getParent() const { return parentTransform; }

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
			globalMatrix = XMMatrixMultiply(localMatrix, parentTransform->getGlobalMatrix());
		} else {
			globalMatrix = localMatrix;
		}
	}

	__inline auto operator= (const XMMATRIX& mat) -> Transform& {
		XMMatrixDecompose(&scale, &rotation, &translation, mat);
		return *this;
	}

	inline void UpdateFromGlobal(const XMMATRIX& newGlobal) {
		XMMATRIX newLocal = (parentTransform != nullptr)
			? newGlobal * XMMatrixInverse(nullptr, newGlobal)
			: newGlobal;
		XMMatrixDecompose(&scale, &rotation, &translation, newLocal);
		computeGlobalMatrix();
	}

	Transform& operator=(const Transform& other) {
		if (this != &other) {
			translation = other.getTranslationVector();
			rotation = other.getRotationQuat();
			scale = other.getScaleVector();
			parentTransform = other.getParent();
			computeGlobalMatrix();
		}
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

	inline void translate(float x, float y, float z, bool updateMatrixes = true) {
		translation = XMVectorAdd(translation, XMVectorSet(x, y, z, 0.0f));
		if (updateMatrixes) { computeGlobalMatrix(true); }
	}

	inline void translate(const XMVECTOR& vec, bool updateMatrixes = true) {
		translation = XMVectorAdd(translation, vec);
		if (updateMatrixes) { computeGlobalMatrix(true); }
	}

	inline void translate(const XMFLOAT3& vec, bool updateMatrixes = true) {
		translation = XMVectorAdd(translation, XMLoadFloat3(&vec));
		if (updateMatrixes) { computeGlobalMatrix(true); }
	}

	inline void setPosition(float x, float y, float z, bool updateMatrixes = true)
	{ 
		translation = XMVectorSet(x, y, z, 0.f); 
		if (updateMatrixes) { computeGlobalMatrix(true); }
	}
	inline void setScale(float x, float y, float z, bool updateMatrixes = true)
	{ 
		scale = XMVectorSet(x, y, z, 0.f); 
		if (updateMatrixes) { computeGlobalMatrix(true); }
	}

	inline void setPosition(const XMFLOAT3& pos, bool updateMatrixes = true)
	{ 
		translation = XMLoadFloat3(&pos); 
		if (updateMatrixes) { computeGlobalMatrix(true); }
	}
	inline void setScale(const XMFLOAT3& s, bool updateMatrixes = true)
	{ 
		scale = XMLoadFloat3(&s); 
		if (updateMatrixes) { computeGlobalMatrix(true); }
	}

	inline void setPositionVector(const XMVECTOR& position, bool updateMatrixes = true)
	{ 
		translation = position; 
		if (updateMatrixes) { computeGlobalMatrix(true); }
	}
	inline void setScaleVector(const XMVECTOR& scale, bool updateMatrixes = true)
	{ 
		this->scale = scale; 
		if (updateMatrixes) { computeGlobalMatrix(true); }
	}
	inline void setRotationQuat(const XMVECTOR& rotation, bool updateMatrixes = true)
	{ 
		this->rotation = rotation; 
		if(updateMatrixes) { computeGlobalMatrix(true); }	
	}

	inline void setEulerAngles(float x, float y, float z, bool updateMatrixes = true)
	{ 
		rotation = XMQuaternionRotationRollPitchYaw(x, y, z);
		if (updateMatrixes) { computeGlobalMatrix(true); }
	}

	inline void setEulerAngles(const XMVECTOR& vec, bool updateMatrixes = true)
	{ 
		rotation = XMQuaternionRotationRollPitchYawFromVector(vec); 
		if (updateMatrixes) { computeGlobalMatrix(true); }
	}

	inline void setEulerX(float x, bool updateMatrixes = true) {
		XMFLOAT3 eulerAngles = QuaternionToEuler(rotation);
		setEulerAngles(x, eulerAngles.y, eulerAngles.z);
		if (updateMatrixes) { computeGlobalMatrix(true); }
	}

	inline void SetEulerY(float y, bool updateMatrixes = true) {
		XMFLOAT3 eulerAngles = QuaternionToEuler(rotation);
		setEulerAngles(eulerAngles.x, y, eulerAngles.z);
		if (updateMatrixes) { computeGlobalMatrix(true); }
	}

	inline void setEulerZ(float z, bool updateMatrixes = true) {
		XMFLOAT3 eulerAngles = QuaternionToEuler(rotation);
		setEulerAngles(eulerAngles.x, eulerAngles.y, z);
		if (updateMatrixes) { computeGlobalMatrix(true); }
	}

	// Add a rotation quat to current transform rotation
	inline void setRotation(const XMVECTOR& deltaRot, bool updateMatrixes = true)
	{
		rotation = XMQuaternionMultiply(deltaRot, rotation);
		if (updateMatrixes) { computeGlobalMatrix(true); }
	};
	// Add a rotation from x,y,z to current transform rotation
	inline void rotate(const XMFLOAT3& eulerDeltaRot, bool updateMatrixes = true)
	{
		XMVECTOR localForward = XMVector3Normalize(XMVector3TransformNormal({ 0,0,1,0 }, localMatrix));
		XMVECTOR localRight = XMVector3Normalize(XMVector3TransformNormal({ 1,0,0,0 }, localMatrix));
		XMVECTOR localUp = XMVector3Normalize(XMVector3TransformNormal({ 0,1,0,0 }, localMatrix));


		// Create quaternions for each component
		XMVECTOR pitchQ = XMQuaternionRotationAxis(localRight, eulerDeltaRot.x);
		XMVECTOR yawQ = XMQuaternionRotationAxis(localUp, eulerDeltaRot.y);
		XMVECTOR rollQ = XMQuaternionRotationAxis(localForward, eulerDeltaRot.z);

		// Apply in order: yaw → pitch → roll
		rotation = XMQuaternionMultiply(yawQ, rotation);   // local or world up?
		rotation = XMQuaternionMultiply(rotation, pitchQ);
		rotation = XMQuaternionMultiply(rotation, rollQ);

		rotation = XMQuaternionNormalize(rotation);

		if (updateMatrixes) { computeGlobalMatrix(true); }
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

	inline void lookAt(XMVECTOR target, bool updateMatrixes = true) {
		XMVECTOR worldPos = XMVector3TransformCoord(translation, parentTransform ? parentTransform->globalMatrix : XMMatrixIdentity());
		XMVECTOR forward = XMVector3Normalize(target - worldPos);
		XMVECTOR up = XMVectorSet(0.f, 1.f, 0.f, 0.f);

		XMVECTOR worldRot = QuatLookAtLH(forward, up);

		if (parentTransform)
			rotation = XMQuaternionMultiply(worldRot, XMQuaternionInverse(parentTransform->getRotationQuat()));
		else
			rotation = worldRot;

		if (updateMatrixes) { computeGlobalMatrix(true); }
	}

	inline bool imGuiRender(const std::string& label, int menuNum = 0, bool editTranslation = true, bool editRotation = true, bool editScale = true) {
		bool transformUpdated = false;
		if (editTranslation && ImGuiDragXMVECTOR3((label + " Translation: ## " + std::to_string(menuNum)).c_str(), translation)) { transformUpdated = true; }
		if (editRotation && ImGuiQuatEulerSlider3Degrees((label + " Rotation: ## " + std::to_string(menuNum)).c_str(), rotation)) 
		{ 
			transformUpdated = true; 
		}
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