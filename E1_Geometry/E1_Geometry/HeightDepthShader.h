#pragma once

#ifndef _CMHEIGHTDEPTHSHADER_H_
#define _CMHEIGHTDEPTHSHADER_H_


#include "DXF.h"

class HeightDepthShader : public BaseShader
{
public:

	HeightDepthShader(ShaderManager* shaderManager, ID3D11Device* device, HWND hwnd);
	~HeightDepthShader();

	void setResources(ID3D11DeviceContext* deviceContext);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);
	ModuleInstance matrixDataModule;
	ModuleInstance lightDepthDataModule;
	ModuleInstance heightMapTextureDataModule;
	ModuleInstance textureSamplerModule;
	ModuleInstance heightMapDataModule;
};

#endif
