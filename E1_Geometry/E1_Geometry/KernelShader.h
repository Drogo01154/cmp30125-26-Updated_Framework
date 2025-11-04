#pragma once

#include "BaseShader.h"
#include "MatrixDataModule.h"

using namespace std;
using namespace DirectX;

enum KernelEffects {
	BOX_BLUR,
	EDGE_DETECTION,
	GAUSSIAN3X3,
	GAUSSIAN5X5,
	SHARPEN
};

class KernelShader : public BaseShader
{
private:
	struct screenDataBufferType
	{
		float texelWidth;
		float texelHeight;
		XMFLOAT2 padding;
	};

public:
	KernelShader(ID3D11Device* device, HWND hwnd, KernelEffects effect, int screenWidth, int screenHeight);
	~KernelShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	MatrixDataModule matrixDataModule;
	ComPtr<ID3D11SamplerState> sampleState;
	ComPtr<ID3D11Buffer> screenDataBuffer;
	float texelWidth;
	float texelHeight;
};