#pragma once
#include "DXF.h"

using namespace std;
using namespace DirectX;

class PointLightDataModule : public BaseShaderModule {
private:
	struct PointLightBufferType {
		XMFLOAT3 lightPosition;
		float zPlane;
	};

public: 
	PointLightDataModule(ID3D11Device* device, HWND hwnd);

	void setModuleParamaters(ID3D11DeviceContext* deviceContext, const XMFLOAT3& lightPosition, float zPlane = 50.f);

	void setResources(D3D11_SHADER_VERSION_TYPE shaderType, ID3D11DeviceContext* deviceContext, size_t startingRegister);
private:
	ComPtr<ID3D11Buffer> PointLightBuffer;
};