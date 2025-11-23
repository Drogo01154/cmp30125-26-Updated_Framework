#include "KernelShader.h"

KernelShader::KernelShader(ShaderManager* shaderManager, ID3D11Device* device, HWND hwnd, KernelEffects effect) : BaseShader(device, hwnd)
{

	switch (effect) {
	case KernelEffects::BOX_BLUR:
		initShader(L"texture_vs.cso", L"BoxBlur_ps.cso");
		break;
	case KernelEffects::EDGE_DETECTION:
		initShader(L"texture_vs.cso", L"EdgeDetection_ps.cso");
		break;
	case KernelEffects::GAUSSIAN3X3:
		initShader(L"texture_vs.cso", L"Gaussian3X3_ps.cso");
		break;
	case KernelEffects::GAUSSIAN5X5:
		initShader(L"texture_vs.cso", L"texture_ps.cso");
		break;
	case KernelEffects::SHARPEN:
		initShader(L"texture_vs.cso", L"Gaussian5X5_ps.cso");
		break;
	}

	matrixDataModule = shaderManager->getShaderModuleID("MatrixDataModule");
	samplerStateModule = shaderManager->getShaderModuleID("RenderTextureSampleModule");
	textureDataModule = shaderManager->getShaderModuleID("TextureDataModule");
	KernelDataModule = shaderManager->getShaderModuleID("KernelDataModule");
}


KernelShader::~KernelShader()
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


void KernelShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);
}

void KernelShader::setResources(ID3D11DeviceContext* deviceContext) {
	//Set vertex shader resources
	matrixDataModule->module->setResources(D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_VERTEX_SHADER, deviceContext, 0);
	samplerStateModule->module->setResources(D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_PIXEL_SHADER, deviceContext, 0);
	textureDataModule->module->setResources(D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_PIXEL_SHADER, deviceContext, 0);
	KernelDataModule->module->setResources(D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_PIXEL_SHADER, deviceContext, 0);
}