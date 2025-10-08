#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

/*
	So we need attenuation - has range?



*/

class LightShader : public BaseShader
{
private:
	struct WorldBufferType
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
	LightShader(ID3D11Device* device, TextureManager* textureManager, HWND hwnd, int maxLights);
	~LightShader();

	void setShaderParamaters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, std::shared_ptr<Material> material, std::vector<std::shared_ptr<Light>>& light, const XMFLOAT4& ambient);

private:
	void initShader(const wchar_t* cs, const wchar_t* ps);

private:
	TextureManager* textureManager;
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* worldDataBuffer;
	ID3D11ShaderResourceView* lightBufferSRV;
	int numLights;
	int maxLights;
};

