#pragma once

#ifndef _SMDEPTHSHADER_H_
#define _SMDEPTHSHADER_H_


#include "DXF.h"

class SMDepthShader : public BaseShader
{

public:

	SMDepthShader(ShaderManager* shaderManager, ID3D11Device* device, HWND hwnd);
	~SMDepthShader();

	void setResources(ID3D11DeviceContext* deviceContext);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);
	ModuleInstance matrixDataModule;
};

#endif

