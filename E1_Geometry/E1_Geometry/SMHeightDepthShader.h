#pragma once

#ifndef _SMHEIGHTDEPTHSHADER_H_
#define _SMHEIGHTDEPTHSHADER_H_


#include "DXF.h"

class SMHeightDepthShader : public BaseShader
{

public:

	SMHeightDepthShader(ShaderManager* shaderManager, ID3D11Device* device, HWND hwnd);
	~SMHeightDepthShader();

	void setResources(ID3D11DeviceContext* deviceContext);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);
	ModuleInstance matrixDataModule;
	ModuleInstance heightMapDataModule;
};

#endif
