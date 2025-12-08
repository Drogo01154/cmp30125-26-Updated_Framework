#include "HeightMapShadowShader.h"

HeightMapShadowShader::HeightMapShadowShader(ShaderManager* shaderManager, ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd) {
	initShader(L"shadowHeightmap_vs.cso", L"shadowHeightmap_ps.cso");
	matrixDataModule = shaderManager->getShaderModuleID("MatrixDataModule");
	lightsDataModule = shaderManager->getShaderModuleID("LightsDataModule");
	materialDataModule = shaderManager->getShaderModuleID("MaterialDataModule");
	
	cameraDataModule = shaderManager->getShaderModuleID("CameraDataModule");
	shadowMapDataModule = shaderManager->getShaderModuleID("ShadowMapDataModule");
	heightMapDataModule = shaderManager->getShaderModuleID("HeightMapDataModule");

	textureSampleStateModule = shaderManager->getShaderModuleID("MaterialTextureSamplerModule");
	shadowMapSamplerStateModule = shaderManager->getShaderModuleID("ShadowMapSamplerModule");
	cubeMapSamplerStateModule = shaderManager->getShaderModuleID("CubeMapSamplerModule");
	heightMapSamplerStateModule = shaderManager->getShaderModuleID("HeightMapSamplerModule");

	diffuseMapTextureModule = shaderManager->getShaderModuleID("TextureDataModule1");
	normalMapTextureModule = shaderManager->getShaderModuleID("TextureDataModule2");
	emissiveMapTextureModule = shaderManager->getShaderModuleID("TextureDataModule3");
	heightMapTextureModule = shaderManager->getShaderModuleID("TextureDataModule4");
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
	heightMapTextureModule->module->setResources(D3D11_SHVER_VERTEX_SHADER, deviceContext, 0);		// Texture2D - T0
	textureSampleStateModule->module->setResources(D3D11_SHVER_VERTEX_SHADER, deviceContext, 0);	// SamplerState - S0
	matrixDataModule->module->setResources(D3D11_SHVER_VERTEX_SHADER, deviceContext, 0);			//Cbuffer - BO
	cameraDataModule->module->setResources(D3D11_SHVER_VERTEX_SHADER, deviceContext, 1);			//Cbuffer - B1
	heightMapDataModule->module->setResources(D3D11_SHVER_VERTEX_SHADER, deviceContext, 2);			// Cbuffer - B2

	//Pixel Shader
	diffuseMapTextureModule->module->setResources(D3D11_SHVER_PIXEL_SHADER, deviceContext, 0);		// Texture2D - T0
	normalMapTextureModule->module->setResources(D3D11_SHVER_PIXEL_SHADER, deviceContext, 1);		// Texture2D - T1
	emissiveMapTextureModule->module->setResources(D3D11_SHVER_PIXEL_SHADER, deviceContext, 2);		// Texture2D - T2


	textureSampleStateModule->module->setResources(D3D11_SHVER_PIXEL_SHADER, deviceContext, 0);		// SamplerState - S0
	cameraDataModule->module->setResources(D3D11_SHVER_PIXEL_SHADER, deviceContext, 0);				// Cbuffer - B0
	materialDataModule->module->setResources(D3D11_SHVER_PIXEL_SHADER, deviceContext, 1);			// Cbuffer - B1
	lightsDataModule->module->setResources(D3D11_SHVER_PIXEL_SHADER, deviceContext, 3);				// Structured Buffer T3

	shadowMapDataModule->module->setResources(D3D11_SHVER_PIXEL_SHADER, deviceContext, 4);			// Structured Buffer - T4, Texture2DArray - T5, TextureCubeArray - T6

	shadowMapSamplerStateModule->module->setResources(D3D11_SHVER_PIXEL_SHADER, deviceContext, 1);	// SamplerState - S1
	cubeMapSamplerStateModule->module->setResources(D3D11_SHVER_PIXEL_SHADER, deviceContext, 2);	// SamplerState - S2
	heightMapSamplerStateModule->module->setResources(D3D11_SHVER_PIXEL_SHADER, deviceContext, 3);	// SamplerState - S3

	
	heightMapTextureModule->module->setResources(D3D11_SHVER_PIXEL_SHADER, deviceContext, 7);		// Texture2D - T7
	heightMapDataModule->module->setResources(D3D11_SHVER_PIXEL_SHADER, deviceContext, 2);			// Cbuffer - B2	
}

