#include "MiniMapTextureShader.h"

MiniMapTextureShader::MiniMapTextureShader(ShaderManager* shaderManager, ID3D11Device* device, HWND hwnd) : 
	BaseShader(device, hwnd) 
{
	initShader(L"texture_vs.cso", L"textureMiniMap_ps.cso");
	matrixDataModule = shaderManager->getShaderModuleID("MatrixDataModule");
	samplerStateModule = shaderManager->getShaderModuleID("RenderTextureSampleModule");
	textureDataModule = shaderManager->getShaderModuleID("TextureDataModule");
	miniMapDataModule = shaderManager->getShaderModuleID("miniMapDataModule");
	/*
	

	
	greyScaleValues = { 0.299, 0.587, 0.114 };
	*/
	

	//Calulcate the orthographic projection matrix
	/*
	
	*/
}

MiniMapTextureShader::~MiniMapTextureShader()
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


void MiniMapTextureShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);
}

void MiniMapTextureShader::setResources(ID3D11DeviceContext* deviceContext) {
	//Set vertex shader resources
	matrixDataModule->module->setResources(D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_VERTEX_SHADER, deviceContext, 0);
	samplerStateModule->module->setResources(D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_PIXEL_SHADER, deviceContext, 0);
	textureDataModule->module->setResources(D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_PIXEL_SHADER, deviceContext, 0);
	miniMapDataModule->module->setResources(D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_PIXEL_SHADER, deviceContext, 0);
}




