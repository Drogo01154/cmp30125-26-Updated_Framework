#pragma once
#include "ShaderManager.h"

class TexturedLightShader : public BaseShader
{
private:

	struct CameraBufferType
	{
		XMFLOAT3 cameraPosition;
		float padding;
	};
public:
	TexturedLightShader(ShaderManager* shaderManager, ID3D11Device* device, HWND hwnd);
	~TexturedLightShader();

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
};

