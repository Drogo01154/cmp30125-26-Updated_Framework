#include "TextureShader.h"

TextureShader::TextureShader(ShaderManager* shaderManager, ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"texture_vs.cso", L"texture_ps.cso");
	matrixDataModule = shaderManager->getShaderModuleID("MatrixDataModule");
	samplerStateModule = shaderManager->getShaderModuleID("RenderTextureSamplerModule");
	textureDataModule = shaderManager->getShaderModuleID("TextureDataModule1");
}


TextureShader::~TextureShader()
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


void TextureShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{

	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

}

void TextureShader::setResources(ID3D11DeviceContext* deviceContext) {
	//Set vertex shader resources
	matrixDataModule->module->setResources(D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_VERTEX_SHADER, deviceContext, 0);
	samplerStateModule->module->setResources(D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_PIXEL_SHADER, deviceContext, 0);
	textureDataModule->module->setResources(D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_PIXEL_SHADER, deviceContext, 0);
}