#pragma once
#pragma once

#include "DXF.h"
#include <wrl/client.h>

using namespace std;
using namespace DirectX;

using Microsoft::WRL::ComPtr;

class HeightMapShader : public BaseShader
{
private:
	struct LightBufferType
	{
		XMFLOAT4 diffuse;
		XMFLOAT3 direction;
		float padding;
	};

	struct MapBufferType
	{
		XMFLOAT2 pixelOffset;
		float heightMultiplier;
		float padding;
	};

public:
	HeightMapShader(ID3D11Device* device, HWND hwnd);
	~HeightMapShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* heightMap, Light* light, float HeightMultiplier);

private:
	void initShader(const wchar_t* cs, const wchar_t* ps);

private:
	ComPtr<ID3D11Buffer> matrixBuffer;
	ComPtr<ID3D11Buffer> mapDataBuffer;
	ComPtr<ID3D11SamplerState> sampleState;
	ComPtr<ID3D11Buffer> lightBuffer;
};

