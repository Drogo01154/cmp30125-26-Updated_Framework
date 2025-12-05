
#pragma once
#include "ShaderManager.h"

class HeightMapShadowShader : public BaseShader {

public:
	HeightMapShadowShader(ShaderManager* shaderManager, ID3D11Device* device, HWND hwnd);
	~HeightMapShadowShader();

	void setResources(ID3D11DeviceContext* deviceContext);

private:
	void initShader(const wchar_t* cs, const wchar_t* ps);

private:
	//Vertex Shader Modules
	ModuleInstance matrixDataModule;	// Shaders Matrix Data
	ModuleInstance cameraDataModule;	// Camera Data Module

	//Pixel Shader Modules
	ModuleInstance lightDataModule;		//Lights data module
	ModuleInstance materialDataModule;	//Material Data module
	ModuleInstance shadowMapDataModule;	//Module for shadow maps

	ModuleInstance textureSamplerStateModule;	// Module for texture sampler
	ModuleInstance shadowMapSamplerStateModule;	// Module for shadow map sampler
	ModuleInstance cubeMapSamplerStateModule;	// Module for cube map sampler
	ModuleInstance albedoTextureDataModule;		// Module for diffuse texture data
	ModuleInstance heightMapTextureDataModule;	// Module for Height map data
	ModuleInstance heightMapDataModule;			// Module for height map data
};
