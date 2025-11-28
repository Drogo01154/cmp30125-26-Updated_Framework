#pragma once

//Utilises buffer b0 for vertex shader

#include "DXF.h"

//Common module for setting up vertex data buffer
class TextureDataModule : public BaseShaderModule {
private:
public:
	TextureDataModule(ID3D11Device* device, HWND hwnd);
	~TextureDataModule();

	void initModule();
	void setModuleParamaters(ID3D11DeviceContext* deviceContext, TextureResource* textureResource);
	void setModuleParamaters(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* texture);
	void setResources(D3D11_SHADER_VERSION_TYPE shaderType, ID3D11DeviceContext* deviceContext, size_t startingRegister);

private:
	ID3D11ShaderResourceView* texturePtr;
};