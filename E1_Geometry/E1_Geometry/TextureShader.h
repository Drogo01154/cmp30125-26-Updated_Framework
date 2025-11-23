#pragma once

#include "DXF.h"

class TextureShader : public BaseShader
{
public:
	TextureShader(ShaderManager* shaderManager, ID3D11Device* device, HWND hwnd);
	~TextureShader();

	void setResources(ID3D11DeviceContext* deviceContext);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	ModuleInstance matrixDataModule;
	ModuleInstance samplerStateModule;
	ModuleInstance textureDataModule;
};

