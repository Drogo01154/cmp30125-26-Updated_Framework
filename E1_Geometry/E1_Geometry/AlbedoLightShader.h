#pragma once
#include "MatrixDataModule.h"
#include "LightsDataModule.h"

class AlbedoLightShader : public BaseShader
{
private:

	struct CameraBufferType
	{
		XMFLOAT3 cameraPosition;
		float padding;
	};
public:
	AlbedoLightShader(ShaderManager* shaderManager, InstanceManager* instanceManager, ID3D11Device* device, HWND hwnd);
	~AlbedoLightShader();

	void setShaderParamaters(
		ID3D11DeviceContext* deviceContext);

private:
	void initShader(const wchar_t* cs, const wchar_t* ps);

private:
	InstanceManager* instanceManager;
	ShaderManager* shaderManager;
	ComPtr<ID3D11Buffer> cameraBuffer;
	std::shared_ptr<MatrixDataModule> matrixDataModule;
	std::shared_ptr<LightsDataModule> lightsDataModule;
};


