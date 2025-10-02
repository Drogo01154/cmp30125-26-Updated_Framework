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
	struct LightBufferType
	{
		XMFLOAT4 attenuation;	// 16
		XMFLOAT4 ambient;		// 16
		XMFLOAT4 diffuse;		// 16

		XMFLOAT3 direction;		// 12
		float innerCone;		// 4

		XMFLOAT3 position;		// 12
		float outerCone;		// 4
	};

public:
	LightShader(ID3D11Device* device, TextureManager* textureManager, HWND hwnd);
	~LightShader();

	void setShaderParamaters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, std::shared_ptr<Material> material, std::shared_ptr<Light> light);

private:
	void initShader(const wchar_t* cs, const wchar_t* ps);

private:
	TextureManager* textureManager;
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* lightBuffer;
};

