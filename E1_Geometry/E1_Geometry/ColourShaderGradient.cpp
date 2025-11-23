// texture shader.cpp
#include "ColourShaderGradient.h"


ColourGradientShader::ColourGradientShader(ShaderManager* shaderManager, ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"colourGradient_vs.cso", L"colourGradient_ps.cso");
}


ColourGradientShader::~ColourGradientShader()
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


void ColourGradientShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;

	// Load (+ compile) shader files
	loadTextureVertexShader(vsFilename);
	loadPixelShader(psFilename);
	
}

void ColourGradientShader::setResources(ID3D11DeviceContext* deviceContext) {
	//Set vertex shader resources
	matrixDataModule->module->setResources(D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_VERTEX_SHADER, deviceContext, 0);
}