#pragma once
#pragma once

#include "DXF.h"
#include <wrl/client.h>
#include "MatrixDataModule.h"

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
		int padding;
	};

	struct MapBufferType
	{
		XMFLOAT2 pixelOffset;
		float heightMultiplier;
		int resolution;
	};

public:
	HeightMapShader(ID3D11Device* device, HWND hwnd);
	~HeightMapShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* heightMap, Light* light, float HeightMultiplier, int resolution);

private:
	void initShader(const wchar_t* cs, const wchar_t* ps);

private:
	MatrixDataModule matrixDataModule;
	ComPtr<ID3D11Buffer> mapDataBuffer;
	ComPtr<ID3D11SamplerState> sampleState;
	ComPtr<ID3D11Buffer> lightBuffer;
};

