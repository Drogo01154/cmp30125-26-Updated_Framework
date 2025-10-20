#pragma once

#include "DXF.h"
#include <wrl/client.h>

using namespace std;
using namespace DirectX;

using Microsoft::WRL::ComPtr;

class ManipulationShader : public BaseShader
{
private:
	struct LightBufferType
	{
		XMFLOAT4 diffuse;
		XMFLOAT3 direction;
		float padding;
	};

	struct TimeBufferType
	{
		float time;
		float speed;
		float amplitude;
		float frequency; 
	};

public:
	ManipulationShader(ID3D11Device* device, HWND hwnd);
	~ManipulationShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, Light* light, float time, float speed, float amplitude, float frequency);

private:
	void initShader(const wchar_t* cs, const wchar_t* ps);

private:
	ComPtr<ID3D11Buffer> matrixBuffer;
	ComPtr<ID3D11Buffer> timeBuffer;
	ComPtr<ID3D11SamplerState> sampleState;
	ComPtr<ID3D11Buffer> lightBuffer;
};

