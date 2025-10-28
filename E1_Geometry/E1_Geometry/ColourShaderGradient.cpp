// texture shader.cpp
#include "ColourShaderGradient.h"


ColourGradientShader::ColourGradientShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd), matrixDataModule(device, hwnd)
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
	
	matrixDataModule.initModule();
}


void ColourGradientShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix)
{
	matrixDataModule.setModuleParamaters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
}