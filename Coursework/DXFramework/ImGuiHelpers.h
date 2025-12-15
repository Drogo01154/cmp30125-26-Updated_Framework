#pragma once

#ifndef _IMGUIHELP_H_
#define _IMGUIHELP_H_

#pragma once

#include "imgui/imgui.h"
#include "MathHelpers.h"
#include <vector>
#include <string>
#include <unordered_map>

using namespace DirectX;

/*
	ImGui helper functions.
*/

//Helper Function for ImGui Slider with XMFLOAT3
inline bool ImGuiXMFloat3Slider(const char* label, XMFLOAT3& vec, 
	float v_min, float v_max, 
	const char* format = "%.3f", 
	ImGuiSliderFlags flags = 0) 
{
	return ImGui::SliderFloat3(label, reinterpret_cast<float*>(&vec.x), v_min, v_max, format, flags);
}

//Helper Function for ImGui Slider with XMVector with three values
inline bool ImGuiXMVECTORSlider3(const char* label, XMVECTOR& vec, 
	float v_min, float v_max, 
	const char* format = "%.3f", 
	ImGuiSliderFlags flags = 0) {
	// Convert vector -> float3
	XMFLOAT3 translation;
	XMStoreFloat3(&translation, vec);

	bool updated = ImGuiXMFloat3Slider(label, translation, v_min, v_max, format, flags);
	
	if (updated) {
		// Convert float3 -> vector
		vec = XMLoadFloat3(&translation);
	}
	return updated;
}

//Helper function for ImGui drag slider with XMFLOAT3
inline bool ImGuiDragXMFLOAT3(const char* label, XMFLOAT3& vec,
	float v_speed = 1.0f, float v_min = 0.0f,
	float v_max = 0.0f, const char* format = "%.3f",
	ImGuiSliderFlags flags = 0) 
{
	return ImGui::DragFloat3(label, &vec.x, v_speed, v_min, v_max, format, flags);
}

//Helper function for ImGui drag slider with XMVector of three values
inline bool ImGuiDragXMVECTOR3(const char* label, XMVECTOR& vec,
	float v_speed = 1.0f, float v_min = 0.0f,
	float v_max = 0.0f, const char* format = "%.3f",
	ImGuiSliderFlags flags = 0) 
{
	// Convert vector -> float3
	XMFLOAT3 translation;
	XMStoreFloat3(&translation, vec);

	bool updated = ImGuiDragXMFLOAT3(label, translation, v_speed, v_min, v_max, format, flags);
	
	if (updated) {
		// Convert float3 -> vector
		vec = XMLoadFloat3(&translation);
	}
	return updated;
}

//Helper function for wrapping degrees -180, 180
inline float WrapDegrees(float deg) {
	deg = fmodf(deg + 180.f, 360.f);
	if (deg < 0.f) deg += 360.f;
	return deg - 180.f;
}

//Helper function for wrapping radians -PI, PI
inline float WrapRadians(float rad) {
	rad = fmodf(rad + XM_PI, XM_2PI);
	if (rad < 0.f) rad += XM_2PI;
	return rad - XM_PI;
}

//Helper function for wrapping XMFLOAT3 values -180, 180 degrees
inline XMFLOAT3 WrapDegrees(const XMFLOAT3& deg) {
	return XMFLOAT3(WrapDegrees(deg.x), WrapDegrees(deg.y), WrapDegrees(deg.z));
}

//Helper function for wrapping XMFLOAT3 value -PI, PI
inline XMFLOAT3 WrapRadians(const XMFLOAT3& rad) {
	return XMFLOAT3(WrapRadians(rad.x), WrapRadians(rad.y), WrapRadians(rad.z));
}

//Helper function for using IMGUI slider in degrees on stored Quaternion
inline bool ImGuiQuatEulerSlider3Degrees(const char* label,
	XMVECTOR& quat,
	float v_min = -180.f, float v_max = 180.f,
	const char* format = "%.3f",
	ImGuiSliderFlags flags = 0)
{
	std::string labelString = std::string(label);
	
	struct cachedRotation {
		XMFLOAT3 eulerDeg;
		XMVECTOR quat;
	};

	//stores values between calls
	static std::unordered_map<std::string, cachedRotation> cache;

	//Convert quaternion -> Euler (degrees)
	XMFLOAT3 rotation = ToDegrees(QuaternionToEuler(quat));

	auto& entry = cache[labelString];

	// Initialize cache if first time or external quat changed significantly
	if (!XMVector3Equal(entry.quat, quat)) {
		entry.quat = quat;
		entry.eulerDeg = ToDegrees(QuaternionToEuler(quat));
	}

	bool updated = ImGuiXMFloat3Slider(label, entry.eulerDeg, v_min, v_max, format, flags);

	if (updated) {
		XMFLOAT3 wrapped = WrapDegrees(entry.eulerDeg);
		XMFLOAT3 radians = ToRadians(wrapped);
		XMVECTOR newQuat = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&radians));

		//Ensure closest quaternion
		if (XMVectorGetX(XMQuaternionDot(newQuat, entry.quat)) < 0.0f) {
			newQuat = XMVectorNegate(newQuat);
		}
			
		entry.quat = newQuat;
		quat = newQuat;
	}
	return updated;
}

//Helper function for using IMGUI slider in radians on stored Quaternion
inline bool ImGuiQuatEulerSlider3Radians(const char* label,
	XMVECTOR& quat,
	float v_min = -XM_PI, float v_max = XM_PI,
	const char* format = "%.3f",
	ImGuiSliderFlags flags = 0) {
	std::string labelString = std::string(label);

	struct cachedRotation {
		XMFLOAT3 eulerRad;
		XMVECTOR quat;
	};

	static std::unordered_map<std::string, cachedRotation> cache;

	//Convert quaternion -> Euler (radians)
	XMFLOAT3 rotation = QuaternionToEuler(quat);

	auto& entry = cache[labelString];

	// Initialize cache if first time or external quat changed significantly
	if (!XMVector3Equal(entry.quat, quat)) {
		entry.quat = quat;
		entry.eulerRad = QuaternionToEuler(quat);
	}

	bool updated = ImGuiXMFloat3Slider(label, entry.eulerRad, v_min, v_max, format, flags);

	if (updated) {
		XMFLOAT3 wrapped = WrapRadians(entry.eulerRad);
		XMVECTOR newQuat = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&wrapped));

		// Continuity: ensure closest quaternion
		if (XMVectorGetX(XMQuaternionDot(newQuat, entry.quat)) < 0.0f)
			newQuat = XMVectorNegate(newQuat);

		entry.quat = newQuat;
		quat = newQuat;
	}
	return updated;
}

#endif