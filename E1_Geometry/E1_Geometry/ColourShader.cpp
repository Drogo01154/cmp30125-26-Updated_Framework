// texture shader.cpp
#include "colourshader.h"
#include "Camera.h"


ColourShader::ColourShader(ShaderManager* shaderManager, ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"colour_vs.cso", L"colour_ps.cso");
	matrixDataModule = shaderManager->getShaderModuleID("MatrixDataModule");
}


ColourShader::~ColourShader()
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


void ColourShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	
	// Load (+ compile) shader files
	loadColourVertexShader(vsFilename);
	loadPixelShader(psFilename);
}

void ColourShader::setResources(ID3D11DeviceContext* deviceContext) {
	//Set vertex shader resources
	matrixDataModule->module->setResources(D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_VERTEX_SHADER, deviceContext, 0);
}




