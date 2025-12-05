#include "CMHeightDepthShader.h"

CMHeightDepthShader::CMHeightDepthShader(ShaderManager* shaderManager, ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"depthHeightmapCM_vs.cso", L"depthCM_ps.cso");
	matrixDataModule = shaderManager->getShaderModuleID("MatrixDataModule");
	pointLightDataModule = shaderManager->getShaderModuleID("PointLightDataModule");
	heightMapDataModule = shaderManager->getShaderModuleID("HeightMapDataModule");
	textureDataModule = shaderManager->getShaderModuleID("HeightMapTextureDataModule");
	samplerStateModule = shaderManager->getShaderModuleID("MaterialTextureSamplerModule");
}


CMHeightDepthShader::~CMHeightDepthShader()
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


void CMHeightDepthShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	// Load (+ compile) shader files
	loadColourVertexShader(vsFilename);
	loadPixelShader(psFilename);
}

void CMHeightDepthShader::setResources(ID3D11DeviceContext* deviceContext) {
	//Set vertex shader resources
	matrixDataModule->module->setResources(D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_VERTEX_SHADER, deviceContext, 0);
	heightMapDataModule->module->setResources(D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_VERTEX_SHADER, deviceContext, 1);
	samplerStateModule->module->setResources(D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_VERTEX_SHADER, deviceContext, 0);
	textureDataModule->module->setResources(D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_VERTEX_SHADER, deviceContext, 0);

	//Set pixel shader resources
	pointLightDataModule->module->setResources(D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_PIXEL_SHADER, deviceContext, 0);
}