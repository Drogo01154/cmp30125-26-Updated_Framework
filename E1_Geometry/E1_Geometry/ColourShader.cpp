// texture shader.cpp
#include "colourshader.h"
#include "Camera.h"


ColourShader::ColourShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd), matrixDataModule(device, hwnd)
{
	initShader(L"colour_vs.cso", L"colour_ps.cso");
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

	matrixDataModule.initModule();
}


void ColourShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix)
{
	matrixDataModule.setModuleParamaters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
}




