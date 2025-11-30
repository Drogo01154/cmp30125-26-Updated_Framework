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

		//Pixel Shader Modules
		ModuleInstance lightDataModule;		//Lights data module
		ModuleInstance materialDataModule;	//Material Data module
		ModuleInstance shadowMapDataModule;	//Module for shadow maps
		
		ModuleInstance textureSamplerStateModule; // Module for texture sampler
		ModuleInstance shadowMapSamplerStateModule; // Module for shadow map samplet
		ModuleInstance textureDataModule;		   // Module for texture data
	};
};