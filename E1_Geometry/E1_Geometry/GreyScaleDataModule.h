#pragma once

#include "DXF.h"

//Common module for setting up vertex data buffer
class GreyScaleDataModule : public BaseShaderModule {
private:
	struct greyScaleBufferType
	{
		XMFLOAT3 greyScale;
		float padding;
	};
public:
	GreyScaleDataModule(ID3D11Device* device, HWND hwnd);

	void initModule();
	void setModuleParamaters(ID3D11DeviceContext* deviceContext, const XMFLOAT3& greyScale = { 0.299, 0.587, 0.114 });
	void setResources(D3D11_SHADER_VERSION_TYPE shaderType, ID3D11DeviceContext* deviceContext, size_t startingRegister);

private:
	ComPtr<ID3D11Buffer> greyScaleBuffer;
};