#pragma once
#include "ShaderManager.h"

class ShadowShader : public BaseShader {

	public:
		ShadowShader(ShaderManager* shaderManager, ID3D11Device* device, HWND hwnd);
		~ShadowShader();

		void setResources(ID3D11DeviceContext* deviceContext);

	private:
		void initShader(const wchar_t* cs, const wchar_t* ps);

	private:
		//Vertex Shader Modules
		ModuleInstance matrixDataModule;	// Shaders Matrix Data
		ModuleInstance cameraDataModule;	// Camera Data Module
		ModuleInstance lightsDataModule;	// light data module
		ModuleInstance materialDataModule;  // Material data module

		//Pixel Shader Modules
		ModuleInstance shadowMapDataModule;	//Module for shadow maps

		ModuleInstance textureSampleStateModule;	// Module for texture sampler
		ModuleInstance shadowMapSamplerStateModule;	// Module for shadow map sampler
		ModuleInstance cubeMapSamplerStateModule;	// Module for cube map sampler

		ModuleInstance diffuseMapTextureModule;		// Module for colour texture
		ModuleInstance normalMapTextureModule;		// Module for normal texture
		ModuleInstance emissiveMapTextureModule;	// Module for emissive texture
};