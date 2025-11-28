#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;



class LightsDataModule : public BaseShaderModule {
private:
	struct LightBufferType
	{
		XMFLOAT4 attenuation;	// 16
		XMFLOAT4 diffuse;		// 16

		XMFLOAT3 direction;		// 12
		float innerCone;		// 4

		XMFLOAT3 position;		// 12
		float outerCone;		// 4
		int type;				// 4
		XMFLOAT3 padding;		// 12
	};

public:
	LightsDataModule(ID3D11Device* device, HWND hwnd, InstanceManager* instanceManager);
	~LightsDataModule();


	void initModule();
	void setModuleParamaters(ID3D11DeviceContext* deviceContext);
	void setResources(D3D11_SHADER_VERSION_TYPE shaderType, ID3D11DeviceContext* deviceContext, size_t startingRegister);

	void setLightsBuffer();
private:
	ComPtr<ID3D11Buffer> lightsBuffer;
	ComPtr<ID3D11ShaderResourceView> lightBufferSRV;
	InstanceManager* instanceManager;

	int numLights;
	int maxLights;

	static constexpr float expandThreshold = 1.0f;   // 100% of capacity
	static constexpr float shrinkThreshold = 0.25f;  // 25% of capacity
};