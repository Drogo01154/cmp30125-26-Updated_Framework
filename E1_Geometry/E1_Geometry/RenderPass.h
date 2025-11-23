#pragma once

#include "../DXFramework/DXF.h"

class renderPass {
public:
	virtual void Render(ID3D11Device* device) = 0;
	virtual ~renderPass() {};
	virtual void ImGuiMenu() {}
	bool hasImGui;
};