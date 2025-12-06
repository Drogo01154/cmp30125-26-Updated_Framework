#include "HeightMapShadowShader.h"

HeightMapShadowShader::HeightMapShadowShader(ShaderManager* shaderManager, ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd) {
	initShader(L"shadowHeightmap_vs.cso", L"shadowHeightmap_ps.cso");
	matrixDataModule = shaderManager->getShaderModuleID("MatrixDataModule");
	lightDataModule = shaderManager->getShaderModuleID("LightsDataModule");
	materialDataModule = shaderManager->getShaderModuleID("MaterialDataModule");
	cameraDataModule = shaderManager->getShaderModuleID("CameraDataModule");
	shadowMapDataModule = shaderManager->getShaderModuleID("ShadowMapDataModule");

	textureSamplerStateModule = shaderManager->getShaderModuleID("MaterialTextureSamplerModule");
	shadowMapSamplerStateModule = shaderManager->getShaderModuleID("ShadowMapSamplerModule");
	cubeMapSamplerStateModule = shaderManager->getShaderModuleID("CubeMapSamplerModule");
	albedoTextureDataModule = shaderManager->getShaderModuleID("TextureDataModule1");
	heightMapTextureDataModule = shaderManager->getShaderModuleID("TextureDataModule2");
	heightMapDataModule = shaderManager->getShaderModuleID("HeightMapDataModule");
}

HeightMapShadowShader::~HeightMapShadowShader() {
	// Release the layout.
	if (layout)
	{
		layout->Release();
		layout = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}

void HeightMapShadowShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);
}
void HeightMapShadowShader::setResources(ID3D11DeviceContext* deviceContext) {
	//Vertex Shader
	heightMapTextureDataModule->module->setResources(D3D11_SHVER_VERTEX_SHADER, deviceContext, 0);	// Texture2D - T0
	textureSamplerStateModule->module->setResources(D3D11_SHVER_VERTEX_SHADER, deviceContext, 0);	// SamplerState - S0
	matrixDataModule->module->setResources(D3D11_SHVER_VERTEX_SHADER, deviceContext, 0);			//Cbuffer - BO
	cameraDataModule->module->setResources(D3D11_SHVER_VERTEX_SHADER, deviceContext, 1);			//Cbuffer - B1
	heightMapDataModule->module->setResources(D3D11_SHVER_VERTEX_SHADER, deviceContext, 2);			// Cbuffer - B2

	//Pixel Shader
	cameraDataModule->module->setResources(D3D11_SHVER_PIXEL_SHADER, deviceContext, 0);				//Cbuffer - B0
	materialDataModule->module->setResources(D3D11_SHVER_PIXEL_SHADER, deviceContext, 1);			//Cbuffer - B1
	lightDataModule->module->setResources(D3D11_SHVER_PIXEL_SHADER, deviceContext, 0);				// Structured Buffer - TO
	shadowMapDataModule->module->setResources(D3D11_SHVER_PIXEL_SHADER, deviceContext, 1);			// Structured Buffer - T1, Texture2DArray - T2, TextureCubeArray - T3
	albedoTextureDataModule->module->setResources(D3D11_SHVER_PIXEL_SHADER, deviceContext, 4);		// Texture2D - T4
	shadowMapSamplerStateModule->module->setResources(D3D11_SHVER_PIXEL_SHADER, deviceContext, 0);	// SamplerState - S0
	cubeMapSamplerStateModule->module->setResources(D3D11_SHVER_PIXEL_SHADER, deviceContext, 1);	// SamplerState - S1
	textureSamplerStateModule->module->setResources(D3D11_SHVER_PIXEL_SHADER, deviceContext, 3);	// SamplerState - S2
	heightMapTextureDataModule->module->setResources(D3D11_SHVER_PIXEL_SHADER, deviceContext, 5);	// Texture2D - T5
	heightMapDataModule->module->setResources(D3D11_SHVER_PIXEL_SHADER, deviceContext, 2);			// Cbuffer - B2
}

