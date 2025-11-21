#pragma once
#pragma once

#include "DXF.h"
#include "SceneGraph.h"

using namespace std;
using namespace DirectX;



class MaterialDataModule : BaseShaderModule {
private:
	struct SceneBufferType
	{
		XMFLOAT4 baseColour;   // 16
		XMFLOAT4 specular;     // 16
		float specularPower;   // 4
		int numberOfLights;    // 4
		XMFLOAT2 padding;      // 8
	};

public:
	MaterialDataModule(ID3D11Device* device, HWND hwnd);
	~MaterialDataModule();

	void setModuleParamaters(ID3D11DeviceContext* deviceContext, Material* material);

	void setResources(ID3D11DeviceContext* deviceContext, size_t startingRegister);

	void initModule();

private:
	ComPtr<ID3D11Buffer> sceneDataBuffer;
};