#pragma once
#include "MatrixDataModule.h"
#include "LightsDataModule.h"

class AlbedoLightShader : public BaseShader
{
private:

	struct CameraBufferType
	{
		XMFLOAT3 cameraPosition;
		float padding;
	};
public:
	AlbedoLightShader(ID3D11Device* device, HWND hwnd);
	~AlbedoLightShader();

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
	ComPtr<ID3D11Buffer> cameraBuffer;
	MatrixDataModule matrixDataModule;
	LightsDataModule lightsDataModule;
};


