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
	//Vertex Shader Modules
	ModuleInstance matrixDataModule;	// Shaders Matrix Data
	ModuleInstance cameraDataModule;	// Camera Data Module
	ModuleInstance lightsDataModule;	// light data module
	ModuleInstance materialDataModule;  // Material data module

	ModuleInstance textureSampleStateModule;	// Module for texture sampler

	ModuleInstance heightMapDataModule;			// Module for heightmap data

	ModuleInstance diffuseMapTextureModule;		// Module for colour texture
	ModuleInstance normalMapTextureModule;		// Module for normal texture
	ModuleInstance emissiveMapTextureModule;	// Module for emissive texture
	ModuleInstance heightMapTextureModule;		// Module for height map texture
	ModuleInstance heightMapSamplerStateModule; // Module for height map sampler
	
};