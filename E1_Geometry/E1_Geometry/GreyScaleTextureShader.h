#pragma once
#include "DXF.h"

class GreyScaleTextureShader : public BaseShader
{
private:
	
public:
	GreyScaleTextureShader(ShaderManager* shaderManager, ID3D11Device* device, HWND hwnd);
	~GreyScaleTextureShader();


	void setResources(ID3D11DeviceContext* deviceContext);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	ShaderManager* shaderManager;
	ModuleInstance matrixDataModule;
	ModuleInstance samplerStateModule;
	ModuleInstance textureDataModule;
	ModuleInstance greyScaleDataModule;
};

