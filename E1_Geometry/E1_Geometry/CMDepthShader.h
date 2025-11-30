#pragma once

// Colour shader.h
// Simple shader example.
#pragma once

#ifndef _CMDEPTHSHADER_H_
#define _CMDEPTHSHADER_H_


#include "DXF.h"

class CMDepthShader : public BaseShader
{
public:

	CMDepthShader(ShaderManager* shaderManager, ID3D11Device* device, HWND hwnd);
	~CMDepthShader();

	void setResources(ID3D11DeviceContext* deviceContext);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);
	ModuleInstance matrixDataModule;
	ModuleInstance pointLightDataModule;
};

#endif