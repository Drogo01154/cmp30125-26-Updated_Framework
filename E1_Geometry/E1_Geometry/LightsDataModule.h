#pragma once

#include "DXF.h"
#include <wrl/client.h>

using namespace std;
using namespace DirectX;
using Microsoft::WRL::ComPtr;



class LightsDataModule : BaseShaderModule {
private:
	struct SceneBufferType
	{
		XMFLOAT4 ambientLight; // 16
		XMFLOAT4 specular;     // 16
		float specularPower;   // 4
		int numberOfLights;    // 4
		XMFLOAT2 padding;      // 8
	};

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
	LightsDataModule(ID3D11Device* device, HWND hwnd);
	~LightsDataModule();

	void setModuleParamaters(ID3D11DeviceContext* deviceContext, 
		std::shared_ptr<Material> material, 
		std::vector<std::shared_ptr<Light>>& lights, 
		const XMFLOAT4& ambient);

	void initModule();

	void setLightsBuffer();
private:
	ComPtr<ID3D11Buffer> lightsBuffer;
	ComPtr<ID3D11Buffer> sceneDataBuffer;
	ComPtr<ID3D11ShaderResourceView> lightBufferSRV;

	int numLights;
	int maxLights;

	static constexpr float expandThreshold = 1.0f;   // 100% of capacity
	static constexpr float shrinkThreshold = 0.25f;  // 25% of capacity
};