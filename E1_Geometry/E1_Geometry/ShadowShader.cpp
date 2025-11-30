#include "ShadowShader.h"

ShadowShader::ShadowShader(ShaderManager* shaderManager, ID3D11Device* device, HWND hwnd) {
	matrixDataModule = shaderManager->getShaderModuleID("MatrixDataModule");
	lightDataModule = shaderManager->getShaderModuleID("LightsDataModule");
	materialDataModule = shaderManager->getShaderModuleID("MaterialDataModule");
	cameraDataModule = shaderManager->getShaderModuleID("CameraDataModule");
	shadowMapDataModule = shaderManager->getShaderModuleID("ShadowMapDataModule");

	textureSamplerStateModule = shaderManager->getShaderModuleID("GeometryTextureSampleModule");
	shadowMapSamplerStateModule = shaderManager->getShaderModuleID("ShadowMapSampleModule");
	textureDataModule = shaderManager->getShaderModuleID("TextureDataModule");
}

ShadowShader::~ShadowShader() {
	// Release the layout.
	if (layout)
	{
		layout->Release();
		layout = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}

void ShadowShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);
}
void ShadowShader::setResources(ID3D11DeviceContext* deviceContext) {
	//Vertex Shader
	matrixDataModule->module->setResources(D3D11_SHVER_VERTEX_SHADER, deviceContext, 0);
	cameraDataModule->module->setResources(D3D11_SHVER_VERTEX_SHADER, deviceContext, 1);

	//Pixel Shader
	cameraDataModule->module->setResources(D3D11_SHVER_PIXEL_SHADER, deviceContext, 0);
	materialDataModule->module->setResources(D3D11_SHVER_PIXEL_SHADER, deviceContext, 1);
	lightDataModule->module->setResources(D3D11_SHVER_PIXEL_SHADER, deviceContext, 1);
	shadowMapDataModule->module->setResources(D3D11_SHVER_PIXEL_SHADER, deviceContext, 2);
	textureDataModule->module->setResources(D3D11_SHVER_PIXEL_SHADER, deviceContext, 5);
	shadowMapSamplerStateModule->module->setResources(D3D11_SHVER_PIXEL_SHADER, deviceContext, 0);
	textureSamplerStateModule->module->setResources(D3D11_SHVER_PIXEL_SHADER, deviceContext, 1);
}

