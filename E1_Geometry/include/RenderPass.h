#pragma once

#ifndef _RENDERPASS_H_
#define _RENDERPASS_H_

#include <d3d11.h>    
#include <dxgi.h>    
#include <d3dcompiler.h> 
#include <unordered_map>
#include <memory>
#include <string>
#include <Nlohmann/json.hpp>

#define passDependancies std::unordered_map<std::string, std::shared_ptr<renderPass>>

/*
	Render Pass class used by render pass mager
	Set up to allow ImGui and parsing functionality
	Map of dependancies allow passes to rely on each other
*/
class renderPass {
public:

	renderPass(size_t stage, const passDependancies&) : passStage(stage), hasImGui(false), jsonString(nullptr)
	{}
	virtual void Render(ID3D11DeviceContext* deviceContext, ID3D11Device* device) = 0;
	virtual void ImGuiMenu() {}
	bool hasImGuiMenu() { return hasImGui; }

	std::string* getJsonString() { return jsonString.get(); }

	virtual void toJson(nlohmann::json& json) = 0;
	virtual void fromJson(const nlohmann::json& json) = 0;
protected:
	size_t passStage;
	bool hasImGui;
	std::unique_ptr<std::string> jsonString;
};

#endif