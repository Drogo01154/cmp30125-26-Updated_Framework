#pragma once

#include "DXF.h"
#include "MatrixDataModule.h"

class HeightMapLightShader : public BaseShader
{
public:
	HeightMapLightShader(ShaderManager* shaderManager, ID3D11Device* device, HWND hwnd);
	~HeightMapLightShader();

	void setResources(ID3D11DeviceContext* deviceContext);

private:
	void initShader(const wchar_t* cs, const wchar_t* ps);

private:
	ModuleInstance matrixDataModule;

	ModuleInstance cameraDataModule;
	ModuleInstance materialDataModule;
	ModuleInstance lightDataModule;

	ModuleInstance samplerStateModule;
	ModuleInstance textureDataModule;
	ModuleInstance heightMapTextureModule;

	ModuleInstance heightMapDataModule;
};