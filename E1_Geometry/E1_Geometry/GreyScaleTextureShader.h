#pragma once

#include "BaseShader.h"
#include "MatrixDataModule.h"

class GreyScaleTextureShader : public BaseShader
{
private:
	struct greyScaleBufferType
	{
		XMFLOAT3 greyScale;
		float padding;
	};
public:
	GreyScaleTextureShader(ID3D11Device* device, HWND hwnd);
	~GreyScaleTextureShader();


	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, const XMFLOAT3& greyScale = { 0.299, 0.587, 0.114 });

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	MatrixDataModule matrixDataModule;
	ComPtr<ID3D11Buffer> greyScaleBuffer;
	ComPtr<ID3D11SamplerState> sampleState;
};

