#include "GreyScaleTextureShader.h"

GreyScaleTextureShader::GreyScaleTextureShader(ShaderManager* shaderManager, ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd), shaderManager(shaderManager)
{
	initShader(L"texture_vs.cso", L"textureGreyscale_ps.cso");
	matrixDataModule = shaderManager->getShaderModuleID("MatrixDataModule");
	samplerStateModule = shaderManager->getShaderModuleID("RenderTextureSampleModule");
	textureDataModule = shaderManager->getShaderModuleID("TextureDataModule1");
	greyScaleDataModule = shaderManager->getShaderModuleID("GreyScaleDataModule");
}


GreyScaleTextureShader::~GreyScaleTextureShader()
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


void GreyScaleTextureShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);
}

void GreyScaleTextureShader::setResources(ID3D11DeviceContext* deviceContext) {
	//Set vertex shader resources
	matrixDataModule->module->setResources(D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_VERTEX_SHADER, deviceContext, 0);
	samplerStateModule->module->setResources(D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_PIXEL_SHADER, deviceContext, 0);
	textureDataModule->module->setResources(D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_PIXEL_SHADER, deviceContext, 0);
	greyScaleDataModule->module->setResources(D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_PIXEL_SHADER, deviceContext, 0);
}