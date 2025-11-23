#pragma once

//Utilises buffer b0 for vertex shader

#include "DXF.h"

//Common module for setting up vertex data buffer
class HeightMapDataModule : BaseShaderModule {
private:
	/** Default world, view, projection matrix buffer struct */
	struct MapBufferType
	{
		XMFLOAT2 pixelOffset;
		float heightMultiplier;
		int resolution;
	};
public:
	HeightMapDataModule(ID3D11Device* device, HWND hwnd);
	~HeightMapDataModule();

	void initModule();
	void setModuleParamaters(ID3D11DeviceContext* deviceContext, Material* material, GeometryData* plane);
	void setResources(D3D11_SHADER_VERSION_TYPE shaderType, ID3D11DeviceContext* deviceContext, size_t startingRegister);
 
private:
	ComPtr<ID3D11Buffer> heightMapBuffer;
};