#include "SMHeightDepthShader.h"


SMHeightDepthShader::SMHeightDepthShader(ShaderManager* shaderManager, ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"depthHeightmapSM_vs.cso", L"depthSM_ps.cso");
	matrixDataModule = shaderManager->getShaderModuleID("MatrixDataModule");
	heightMapDataModule = shaderManager->getShaderModuleID("HeightMapDataModule");
}


SMHeightDepthShader::~SMHeightDepthShader()
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


void SMHeightDepthShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	// Load (+ compile) shader files
	loadColourVertexShader(vsFilename);
	loadPixelShader(psFilename);
}

void SMHeightDepthShader::setResources(ID3D11DeviceContext* deviceContext) {
	//Set vertex shader resources
	matrixDataModule->module->setResources(D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_VERTEX_SHADER, deviceContext, 0);
	heightMapDataModule->module->setResources(D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_VERTEX_SHADER, deviceContext, 1);
}