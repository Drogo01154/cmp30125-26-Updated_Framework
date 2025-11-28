// Colour shader.h
// Simple shader example.
#pragma once

#ifndef _DEPTHSHADER_H_
#define _DEPTHSHADER_H_


#include "DXF.h"

class DepthShader : public BaseShader
{

public:

	DepthShader(ShaderManager* shaderManager, ID3D11Device* device, HWND hwnd);
	~DepthShader();

	void setResources(ID3D11DeviceContext* deviceContext);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);
	ModuleInstance matrixDataModule;
};

#endif

