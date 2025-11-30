#pragma once

#include "DXF.h"
#include "SceneGraph.h"

using namespace std;
using namespace DirectX;

class MaterialDataModule : public BaseShaderModule {
private:
	struct MaterialBufferType
	{
		XMFLOAT4 baseColour;   // 16
		XMFLOAT4 specular;     // 16
		float specularPower;   // 4
		XMFLOAT3 padding;      // 8
	};

public:
	MaterialDataModule(ID3D11Device* device, HWND hwnd);

	void setModuleParamaters(ID3D11DeviceContext* deviceContext, Material* material);
	void setResources(D3D11_SHADER_VERSION_TYPE shaderType, ID3D11DeviceContext* deviceContext, size_t startingRegister);

private:
	ComPtr<ID3D11Buffer> MaterialDataBuffer;
};