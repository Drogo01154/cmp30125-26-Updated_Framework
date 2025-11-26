#pragma once

#include "DXF.h"

class renderPass {
public:
	virtual void Render(ID3D11DeviceContext* deviceContext, ID3D11Device* device, const XMMATRIX& projectionMatrix) = 0;
	virtual void ImGuiMenu() {}
	bool hasImGui;
};

/*
	Render pass will require two functions:
		1: render, renders result of pass to render target or its required data?

*/