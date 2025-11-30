#pragma once

#include "DXF.h"

//Common module for setting up vertex data buffer
class KernelDataModule : public BaseShaderModule {
private:
	struct screenDataBufferType
	{
		float texelWidth;
		float texelHeight;
		XMFLOAT2 padding;
	};
public:
	KernelDataModule(ID3D11Device* device, HWND hwnd, int screenWidth, int screenHeight);
	void setModuleParamaters(ID3D11DeviceContext* deviceContext);
	void setResources(D3D11_SHADER_VERSION_TYPE shaderType, ID3D11DeviceContext* deviceContext, size_t startingRegister);

private:
	ComPtr<ID3D11Buffer> screenDataBuffer;
	float texelWidth;
	float texelHeight;
};


