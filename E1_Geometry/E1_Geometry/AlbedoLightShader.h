#pragma once
#include "DXF.h"


class AlbedoLightShader : public BaseShader
{
private:

	struct CameraBufferType
	{
		XMFLOAT3 cameraPosition;
		float padding;
	};
public:
	AlbedoLightShader(ShaderManager* shaderManager, ID3D11Device* device, HWND hwnd);
	~AlbedoLightShader();

	void setResources(ID3D11DeviceContext* deviceContext);

private:
	void initShader(const wchar_t* cs, const wchar_t* ps);

private:
	InstanceManager* instanceManager;
	ShaderManager* shaderManager;
	ModuleInstance MatrixDataModule;
	ModuleInstance LightDataModule;
	ModuleInstance MaterialDataModule;
	ModuleInstance LightDataModule;
	ModuleInstance CameraDataModule;
};


