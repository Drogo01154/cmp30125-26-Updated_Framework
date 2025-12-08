#pragma once
#include "DXF.h"

using namespace std;
using namespace DirectX;

class LightDepthDataBuffer : public BaseShaderModule {
private:
	struct DepthDataBufferType {
		XMFLOAT3 lightPosition;
		float farPlane;
		int lightType;
		XMFLOAT3 padding;
	};

public: 
	LightDepthDataBuffer(ID3D11Device* device, HWND hwnd);

	void setModuleParamaters(ID3D11DeviceContext* deviceContext, Light* light);

	void setResources(D3D11_SHADER_VERSION_TYPE shaderType, ID3D11DeviceContext* deviceContext, size_t startingRegister);
private:
	ComPtr<ID3D11Buffer> DepthDataBuffer;
};