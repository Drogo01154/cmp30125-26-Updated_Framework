#pragma once

#include "LightsModule.h"

/*
	So we need attenuation - has range?



*/

class LightTextureShader : public BaseShader
{
private:

	struct CameraBufferType
	{
		XMFLOAT3 cameraPosition;
		float padding;
	};
public:
	LightTextureShader(ID3D11Device* device, TextureManager* textureManager, HWND hwnd);
	~LightTextureShader();

	void setShaderParamaters(
		ID3D11DeviceContext* deviceContext, 
		const XMMATRIX& world, 
		const XMMATRIX& view, 
		const XMMATRIX& projection, 
		std::shared_ptr<Material> material, 
		std::vector<std::shared_ptr<Light>>& light, 
		const XMFLOAT4& ambient,
		Camera* camera);

private:
	void initShader(const wchar_t* cs, const wchar_t* ps);

private:
	TextureManager* textureManager;
	ComPtr<ID3D11Buffer> cameraBuffer;
	ComPtr<ID3D11Buffer> matrixBuffer;
	ComPtr<ID3D11SamplerState> sampleState;
	
	LightsModule lightsModule;
};

