#pragma once

#include "BaseShader.h"
#include "MatrixDataModule.h"

enum KernelEffects {
	BOX_BLUR,
	EDGE_DETECTION,
	GAUSSIAN3X3,
	GAUSSIAN5X5,
	SHARPEN
};

class KernelShader : public BaseShader
{
private:
	

public:
	KernelShader(ShaderManager* shaderManager, ID3D11Device* device, HWND hwnd, KernelEffects effect);
	~KernelShader();

	void setResources(ID3D11DeviceContext* deviceContext);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

	ModuleInstance matrixDataModule;
	ModuleInstance samplerStateModule;
	ModuleInstance KernelDataModule;
	ModuleInstance textureDataModule;
};