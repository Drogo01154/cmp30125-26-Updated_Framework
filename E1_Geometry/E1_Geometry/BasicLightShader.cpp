#include "BasicLightShader.h"

BasicLightShader::BasicLightShader(ShaderManager* shaderManager, ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"light_vs.cso", L"light_ps.cso");
	matrixDataModule = shaderManager->getShaderModuleID("MatrixDataModule");
	lightsDataModule = shaderManager->getShaderModuleID("LightsDataModule");
	materialDataModule = shaderManager->getShaderModuleID("MaterialDataModule");

	cameraDataModule = shaderManager->getShaderModuleID("CameraDataModule");

	textureSampleStateModule = shaderManager->getShaderModuleID("MaterialTextureSamplerModule");
	diffuseMapTextureModule = shaderManager->getShaderModuleID("TextureDataModule1");
	normalMapTextureModule = shaderManager->getShaderModuleID("TextureDataModule2");
	emissiveMapTextureModule = shaderManager->getShaderModuleID("TextureDataModule3");
}


BasicLightShader::~BasicLightShader()
{
	// Release the layout.
	if (layout)
	{
		layout->Release();
		layout = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}


void BasicLightShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);
}

void BasicLightShader::setResources(ID3D11DeviceContext* deviceContext) {
	//Vertex Shader
	textureSampleStateModule->module->setResources(D3D11_SHVER_VERTEX_SHADER, deviceContext, 0);	// SamplerState - S0
	matrixDataModule->module->setResources(D3D11_SHVER_VERTEX_SHADER, deviceContext, 0);			//Cbuffer - BO
	cameraDataModule->module->setResources(D3D11_SHVER_VERTEX_SHADER, deviceContext, 1);			//Cbuffer - B1

	//Pixel Shader
	diffuseMapTextureModule->module->setResources(D3D11_SHVER_PIXEL_SHADER, deviceContext, 0);		// Texture2D - T0
	normalMapTextureModule->module->setResources(D3D11_SHVER_PIXEL_SHADER, deviceContext, 1);		// Texture2D - T1
	emissiveMapTextureModule->module->setResources(D3D11_SHVER_PIXEL_SHADER, deviceContext, 2);		// Texture2D - T2
	

	textureSampleStateModule->module->setResources(D3D11_SHVER_PIXEL_SHADER, deviceContext, 0);		// SamplerState - S0
	cameraDataModule->module->setResources(D3D11_SHVER_PIXEL_SHADER, deviceContext, 0);				// Cbuffer - B0
	materialDataModule->module->setResources(D3D11_SHVER_PIXEL_SHADER, deviceContext, 1);			// Cbuffer - B1
	lightsDataModule->module->setResources(D3D11_SHVER_PIXEL_SHADER, deviceContext, 3);				// Structured Buffer T3
}