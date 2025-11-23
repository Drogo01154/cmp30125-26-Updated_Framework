#pragma once

#include "DXF.h"

class MiniMapTextureShader : public BaseShader
{
public:
	MiniMapTextureShader(ShaderManager* shaderManager, ID3D11Device* device, HWND hwnd);
	~MiniMapTextureShader();

	void setResources(ID3D11DeviceContext* deviceContext);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

	ModuleInstance matrixDataModule;
	ModuleInstance samplerStateModule;
	ModuleInstance textureDataModule;
	ModuleInstance miniMapDataModule;
};

