#pragma once

#include "DXF.h"

class renderPass {
public:
	virtual void Render(ID3D11Device* device) = 0;
	virtual ~renderPass() {};
	virtual void ImGuiMenu() {}
	bool hasImGui;
};

/*
	Render pass will require two functions:
		1: render, renders result of pass to render target or its required data?

*/