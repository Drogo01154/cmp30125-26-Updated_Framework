#pragma once

#ifndef _RENDERPASS_H_
#define _RENDERPASS_H_

#include <d3d11.h>    
#include <dxgi.h>    
#include <d3dcompiler.h> 
#include <unordered_map>
#include <memory>

#define passDependancies std::unordered_map<std::string, std::shared_ptr<renderPass>>

class renderPass {
public:

	renderPass(size_t stage, const passDependancies&) { passStage = stage; }
	virtual void Render(ID3D11DeviceContext* deviceContext, ID3D11Device* device) = 0;
	virtual void ImGuiMenu() {}
private:
	size_t passStage;
	bool hasImGui;
};

#endif