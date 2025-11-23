#pragma once

#include "DXF.h"
#include "MatrixDataModule.h"

class ManipulationShader : public BaseShader
{
private:
public:
	ManipulationShader(ShaderManager* shaderManager, ID3D11Device* device, HWND hwnd);
	~ManipulationShader();

	void setResources(ID3D11DeviceContext* deviceContext);

private:
	void initShader(const wchar_t* cs, const wchar_t* ps);

private:
	ModuleInstance matrixDataModule;
	ModuleInstance lightDataModule;
	ModuleInstance materialDataModule;
	ModuleInstance cameraDataModule;
	ModuleInstance sampleStateModule;
	ModuleInstance textureDataModule;
	ModuleInstance manipulationDataModule;
};

