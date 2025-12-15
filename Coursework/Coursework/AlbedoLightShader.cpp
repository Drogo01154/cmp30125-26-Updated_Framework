#include "albedoLightShader.h"

#include "MatrixDataModule.h"
#include "LightsDataModule.h"
AlbedoLightShader::AlbedoLightShader(ShaderManager* shaderManager, ID3D11Device* device, HWND hwnd) :
	BaseShader(device, hwnd)

{
	initShader(L"light_vs.cso", L"albedoLight_ps.cso");
	matrixDataModule = shaderManager->getShaderModuleID("MatrixDataModule");
	lightDataModule = shaderManager->getShaderModuleID("LightsDataModule");
	materialDataModule = shaderManager->getShaderModuleID("MaterialDataModule");
	cameraDataModule = shaderManager->getShaderModuleID("CameraDataModule");
}


AlbedoLightShader::~AlbedoLightShader()
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


void AlbedoLightShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);
}

void AlbedoLightShader::setResources(ID3D11DeviceContext* deviceContext)
{
	//Set vertex shader resources
	matrixDataModule->module->setResources(D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_VERTEX_SHADER, deviceContext, 0);
	
	//Set pixel shaders cBuffers
	cameraDataModule->module->setResources(D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_PIXEL_SHADER, deviceContext, 0);
	materialDataModule->module->setResources(D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_PIXEL_SHADER, deviceContext, 1);
	
	//Set light SRVs
	lightDataModule->module->setResources(D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_PIXEL_SHADER, deviceContext, 0);
}