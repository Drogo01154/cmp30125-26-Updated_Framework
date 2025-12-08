#include "HeightDepthShader.h"

HeightDepthShader::HeightDepthShader(ShaderManager* shaderManager, ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"depthHeightmap_vs.cso", L"depth_ps.cso");
	matrixDataModule = shaderManager->getShaderModuleID("MatrixDataModule");
	lightDepthDataModule = shaderManager->getShaderModuleID("LightDepthDataModule");
	heightMapDataModule = shaderManager->getShaderModuleID("HeightMapDataModule");
	heightMapTextureDataModule = shaderManager->getShaderModuleID("TextureDataModule4");
	textureSamplerModule = shaderManager->getShaderModuleID("HeightMapSamplerModule");
}


HeightDepthShader::~HeightDepthShader()
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


void HeightDepthShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	// Load (+ compile) shader files
	loadColourVertexShader(vsFilename);
	loadPixelShader(psFilename);
}

void HeightDepthShader::setResources(ID3D11DeviceContext* deviceContext) {
	//Set vertex shader resources
	matrixDataModule->module->setResources(D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_VERTEX_SHADER, deviceContext, 0);
	heightMapDataModule->module->setResources(D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_VERTEX_SHADER, deviceContext, 1);
	textureSamplerModule->module->setResources(D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_VERTEX_SHADER, deviceContext, 0);
	heightMapTextureDataModule->module->setResources(D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_VERTEX_SHADER, deviceContext, 0);

	//Set pixel shader resources
	lightDepthDataModule->module->setResources(D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_PIXEL_SHADER, deviceContext, 0);
}