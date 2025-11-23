#include "ManipulationShader.h"

ManipulationShader::ManipulationShader(ShaderManager* shaderManager, ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"manipulation_vs.cso", L"manipulation_ps.cso");

	matrixDataModule = shaderManager->getShaderModuleID("MatrixDataModule");
	lightDataModule = shaderManager->getShaderModuleID("LightsDataModule");
	materialDataModule = shaderManager->getShaderModuleID("MaterialDataModule");
	cameraDataModule = shaderManager->getShaderModuleID("CameraDataModule");
	sampleStateModule = shaderManager->getShaderModuleID("GeometryTextureSampleModule");
	textureDataModule = shaderManager->getShaderModuleID("TextureDataModule");
	manipulationDataModule = shaderManager->getShaderModuleID("ManipulationDataModule");
}


ManipulationShader::~ManipulationShader()
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

void ManipulationShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);
}

void ManipulationShader::setResources(ID3D11DeviceContext* deviceContext) {
	//Set vertex shader resources
	matrixDataModule->module->setResources(D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_VERTEX_SHADER, deviceContext, 0);
	manipulationDataModule->module->setResources(D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_VERTEX_SHADER, deviceContext, 1);

	//Set pixel shaders cBuffers
	cameraDataModule->module->setResources(D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_PIXEL_SHADER, deviceContext, 0);
	materialDataModule->module->setResources(D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_PIXEL_SHADER, deviceContext, 1);


	//Set light SRVs
	lightDataModule->module->setResources(D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_PIXEL_SHADER, deviceContext, 0);
	textureDataModule->module->setResources(D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_PIXEL_SHADER, deviceContext, 1);

	//Set samplers
	sampleStateModule->module->setResources(D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_PIXEL_SHADER, deviceContext, 0);
}
