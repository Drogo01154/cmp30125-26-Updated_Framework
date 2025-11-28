#pragma once

#include "DXF.h"

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

/*
	Render pass will require two functions:
		1: render, renders result of pass to render target or its required data?

*/