#include "albedoLightShader.h"

#include "MatrixDataModule.h"
#include "LightsDataModule.h"
AlbedoLightShader::AlbedoLightShader(ShaderManager* shaderManager, ID3D11Device* device, HWND hwnd) :
	BaseShader(device, hwnd),
	shaderManager(shaderManager),
	instanceManager(instanceManager)

{
	MatrixDataModule = shaderManager->getShaderModuleID("MatrixDataModule");
	LightDataModule = shaderManager->getShaderModuleID("LightDataModule");
	MaterialDataModule = shaderManager->getShaderModuleID("MaterialDataModule");
	CameraDataModule = shaderManager->getShaderModuleID("CameraDataModule");
	initShader(L"light_vs.cso", L"albedoLight_ps.cso");
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
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC cameraBufferDesc;

	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);
}

void AlbedoLightShader::setShaderParamaters(ID3D11DeviceContext* deviceContext)
{
	//Set vertex shader resources
	MatrixDataModule->module->setResources(D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_VERTEX_SHADER, deviceContext, 0);
	
	//Set pixel shaders cBuffers
	CameraDataModule->module->setResources(D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_PIXEL_SHADER, deviceContext, 0);
	MaterialDataModule->module->setResources(D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_PIXEL_SHADER, deviceContext, 1);
	
	//Set light SRVs
	LightDataModule->module->setResources(D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_PIXEL_SHADER, deviceContext, 0);
}