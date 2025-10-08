#pragma once

#include "imgui/imgui.h"
#include <vector>
#include <string>

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