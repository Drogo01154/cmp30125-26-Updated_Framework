#pragma once
#include "ShaderManager.h"

class BasicLightShader : public BaseShader
{
private:
public:
	BasicLightShader(ShaderManager* shaderManager, ID3D11Device* device, HWND hwnd);
	~BasicLightShader();

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

	ModuleInstance diffuseMapTextureModule;		// Module for colour texture
	ModuleInstance normalMapTextureModule;		// Module for normal texture
	ModuleInstance emissiveMapTextureModule;	// Module for emissive texture
};

