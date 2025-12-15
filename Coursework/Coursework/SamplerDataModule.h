#pragma once

//Utilises buffer b0 for vertex shader

#include "DXF.h"

//Common module for setting up vertex data buffer
class SamplerDataModule : public BaseShaderModule {
private:
public:
	SamplerDataModule(ID3D11Device* device, HWND hwnd,
		D3D11_TEXTURE_ADDRESS_MODE textureMode = D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_FILTER filter = D3D11_FILTER_ANISOTROPIC,
		UINT maxAnisotropy = 1,
		D3D11_COMPARISON_FUNC compFunc = D3D11_COMPARISON_ALWAYS,
		const float* borderColor = nullptr);

	void setResources(D3D11_SHADER_VERSION_TYPE shaderType, ID3D11DeviceContext* deviceContext, size_t startingRegister);
private:
	ComPtr<ID3D11SamplerState> samplerState;
	D3D11_TEXTURE_ADDRESS_MODE textureMode;
	D3D11_FILTER filter;
	UINT maxAnisotropy;
	D3D11_COMPARISON_FUNC compFunc;
};