#pragma once

#ifndef _IMGUIHELP_H_
#define _IMGUIHELP_H_

#pragma once

#include "imgui/imgui.h"
#include <vector>
#include <string>
#include "Transform.h"

using namespace DirectX;

enum buttonSizes
{
	SmallButton,
	NormalButton,
	LargeButton,
	VerticalDummy,
	SelectableSideButton,
	ComboSideButton,
};

class UIConstants
{
	inline static ImGuiStyle baseStyle;										// Saved ImGuiStyle on initialisation
	inline static const float baseWindowWidth = 630.f;						// Base width of window
	inline static const float WindowBorder = 10.f;							// Base estimated value of window border
	inline static const float BaseComboButtonWidth = 206.f;					// Base width of combo side button
	inline static std::vector<ImVec2> UISizes;								// Vector of final Element sizes
	inline static ImVec2 previousWindowSize;								// Previous Window Size;
public:

	//Getters for UI element standards
	inline static const ImVec2& getSmallButtonSize() { return UISizes[SmallButton]; }
	inline static const ImVec2& getButtonSize() { return UISizes[NormalButton]; }
	inline static const ImVec2& getLargeButtonSize() { return UISizes[LargeButton]; }
	inline static const ImVec2& getVertDummySize() { return UISizes[VerticalDummy]; }
	inline static const ImVec2& getSelectableSideButtonSize() { return UISizes[SelectableSideButton]; }
	inline static const ImVec2& getComboButtonSize() { return UISizes[ComboSideButton]; }

	//Function for initialising system
	inline static void InitialiseSystem()
	{
		UISizes.resize(buttonSizes::ComboSideButton + 1);	//Set vector to correct size
		previousWindowSize = { 0.f, 0.f };
	}

	//Function for updating sizes based on current ImGui window size
	inline static void UpdateConstants(int sWidth, int sHeight)
	{
		if (!ImGui::GetCurrentContext()) return; // Ensure ImGui is initialized

		ImVec2 windowSize = { (float)sWidth, (float)sHeight };
		if (windowSize.x != previousWindowSize.x)
		{
			previousWindowSize = windowSize;
			float scaleFactor = windowSize.x / baseWindowWidth;
			float MaxButtonSize = windowSize.x - (WindowBorder * scaleFactor);

			float yHeight = ImGui::GetFrameHeight();

			UISizes[SmallButton] = { MaxButtonSize / 3.f, yHeight };
			UISizes[NormalButton] = { MaxButtonSize / 2.f, yHeight };
			UISizes[LargeButton] = { MaxButtonSize, yHeight };
			UISizes[VerticalDummy] = { 0.f, yHeight };
			UISizes[SelectableSideButton] = { MaxButtonSize - UISizes[SmallButton].x, yHeight };
			UISizes[ComboSideButton] = { BaseComboButtonWidth * scaleFactor, yHeight };
		}
	}
};

//Helper function combined combo list and button functionality
inline bool ComboButton(const char* label, int* current_item, const std::vector<const char*>& items)
{
	bool changed = ImGui::Combo((std::string("##") + label).c_str(), current_item, items.data(), items.size());
	ImGui::SameLine();
	if (ImGui::Button(label, UIConstants::getComboButtonSize())) { return *current_item != -1; }
	return false;
}

inline bool ArrowIterators(const std::string& _label, uint32_t& _currentItem, uint32_t _containerSize, const std::string& id = "0")
{
	bool iterated = false;
	if (_containerSize > 1) // If more than 1 shape allowing switching through arrow buttons
	{
		if (ImGui::ArrowButton(("shape left##" + id).c_str(), ImGuiDir_Left)) {
			_currentItem = (_currentItem == 0) ? _containerSize - 1 : _currentItem - 1;
			iterated = true;
		}
		ImGui::SameLine();
		ImGui::Selectable((_label + ": " + std::to_string(_currentItem)).c_str(), false, ImGuiSelectableFlags_Disabled, UIConstants::getButtonSize());
		ImGui::SameLine();
		if (ImGui::ArrowButton(("shape right##" + id).c_str(), ImGuiDir_Right)) {
			_currentItem = (_currentItem == _containerSize - 1) ? 0 : _currentItem + 1;
			iterated = true;
		}
	}
	return iterated;
}

inline bool ImGuiXMFloat3Slider(const char* label, XMFLOAT3& vec, 
	float v_min, float v_max, 
	const char* format = "%.3f", 
	ImGuiSliderFlags flags = 0) 
{
	return ImGui::SliderFloat3(label, reinterpret_cast<float*>(&vec.x), v_min, v_max, format, flags);
}

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

inline bool ImGuiDragXMFLOAT3(const char* label, XMFLOAT3& vec,
	float v_speed = 1.0f, float v_min = 0.0f,
	float v_max = 0.0f, const char* format = "%.3f",
	ImGuiSliderFlags flags = 0) 
{
	return ImGui::DragFloat3(label, &vec.x, v_speed, v_min, v_max, format, flags);
}

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

inline float WrapDegrees(float deg) {
	deg = fmodf(deg + 180.f, 360.f);
	if (deg < 0.f) deg += 360.f;
	return deg - 180.f;
}

inline float WrapRadians(float rad) {
	rad = fmodf(rad + XM_PI, XM_2PI);
	if (rad < 0.f) rad += XM_2PI;
	return rad - XM_PI;
}

inline XMFLOAT3 WrapDegrees(const XMFLOAT3& deg) {
	return XMFLOAT3(WrapDegrees(deg.x), WrapDegrees(deg.y), WrapDegrees(deg.z));
}

inline XMFLOAT3 WrapRadians(const XMFLOAT3& deg) {
	return XMFLOAT3(WrapRadians(deg.x), WrapRadians(deg.y), WrapRadians(deg.z));
}

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

		// Continuity: ensure closest quaternion
		if (XMVectorGetX(XMQuaternionDot(newQuat, entry.quat)) < 0.0f)
			newQuat = XMVectorNegate(newQuat);

		entry.quat = newQuat;
		quat = newQuat;
	}
	return updated;
}

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

inline bool TransformImGui(const std::string& label, Transform& transform, bool editScale = true, int menuNum = 0) {
	bool transformUpdated = false;
	if (ImGuiDragXMVECTOR3(("Translation: ## " + std::to_string(menuNum)).c_str(), transform.translation)) { transformUpdated = true; }
	if (ImGuiQuatEulerSlider3Degrees(("Rotation: ## " + std::to_string(menuNum)).c_str(), transform.rotation)) { transformUpdated = true; }
	if (editScale && ImGuiDragXMVECTOR3(("Scale: ## " + std::to_string(menuNum)).c_str(), transform.scale)) { transformUpdated = true; }
	return transformUpdated;
}

#endif