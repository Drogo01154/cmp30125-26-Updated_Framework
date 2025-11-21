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

	void setShaderParamaters(ID3D11DeviceContext* deviceContext, GeometryData& geometryData);

private:
	void initShader(const wchar_t* cs, const wchar_t* ps);

private:
	InstanceManager* instanceManager;
	ShaderManager* shaderManager;
	ComPtr<ID3D11Buffer> cameraBuffer;
	ModuleInstance MatrixDataModule;
	ModuleInstance LightDataModule;
	ModuleInstance MaterialDataModule;
};


