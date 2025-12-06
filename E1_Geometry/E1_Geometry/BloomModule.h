#pragma once
#include "DXF.h"

using namespace std;
using namespace DirectX;

class BloomDataModule : public BaseShaderModule {
private:
	struct BloomDataBufferType {
		float threshold;
		float intensity;
		XMFLOAT2 padding;
	};

public:
	BloomDataModule(ID3D11Device* device, HWND hwnd);

	void setModuleParamaters(ID3D11DeviceContext* deviceContext, float theshold, float intensity);

	void setResources(D3D11_SHADER_VERSION_TYPE shaderType, ID3D11DeviceContext* deviceContext, size_t startingRegister);
private:
	ComPtr<ID3D11Buffer> PointLightBuffer;
};
