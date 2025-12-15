#pragma once

//Utilises buffer b0 for vertex shader

#include "DXF.h"

//Common module for setting up vertex data buffer
class HeightMapDataModule : public BaseShaderModule {
private:
	struct MapBufferType
	{
		XMFLOAT2 offset;              // 8
		float heightMultiplier;       // 4
		float heightPadding;		  // 4
		XMFLOAT2 worldStep;           // 8
		float UVScale;				  // 4
		float padding;				  // 4
	};
public:
	HeightMapDataModule(ID3D11Device* device, HWND hwnd);

	void setModuleParamaters(ID3D11DeviceContext* deviceContext, Material* material, GeometryData* plane);
	void setResources(D3D11_SHADER_VERSION_TYPE shaderType, ID3D11DeviceContext* deviceContext, size_t startingRegister);
 
private:
	ComPtr<ID3D11Buffer> heightMapBuffer;
};