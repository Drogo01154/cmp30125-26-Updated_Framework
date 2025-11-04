#pragma once

#include "BaseShader.h"
#include "MatrixDataModule.h"

using namespace std;
using namespace DirectX;

class MiniMapTextureShader : public BaseShader
{
private:
	struct miniMapDataType
	{
		XMFLOAT4 playerIconColour;
		XMFLOAT3 greyScaleValues;
		float playerIconRadius;
		XMFLOAT3 cameraPos;
		float padding;
	};

public:
	MiniMapTextureShader(ID3D11Device* device, HWND hwnd, int screenWidth, int screenHeight);
	~MiniMapTextureShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext,
		const XMMATRIX& world,
		const XMMATRIX& view,
		const XMMATRIX& projection,
		ID3D11ShaderResourceView* texture,
		Camera* camera,
		Camera* miniMapCamera);

	void ImGuiMenu();

	inline const XMMATRIX& getOrthographicMatrix() { return orthographicProjectionMatrix; }

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	MatrixDataModule matrixDataModule;
	ComPtr<ID3D11Buffer> miniMapBuffer;
	ComPtr<ID3D11SamplerState> sampleState;

	float miniMapWidth;
	float miniMapHeight;
	float miniMapNearZ;
	float miniMapFarZ;

	XMFLOAT4 playerIconColour;
	XMFLOAT3 greyScaleValues;
	float playerIconRadius;
	XMMATRIX orthographicProjectionMatrix;

	XMFLOAT2 screenSize;
};