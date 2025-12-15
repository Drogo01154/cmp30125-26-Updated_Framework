#pragma once

#pragma once

//Utilises buffer b0 for vertex shader

#include "DXF.h"

//Common module for setting up vertex data buffer
class MiniMapDataModule : public BaseShaderModule {
private:
	struct miniMapDataType
	{
		XMFLOAT4 playerIconColour;
		XMFLOAT2 projectedScreenPos;
		float playerIconRadius;
		float padding;
	};
public:
	MiniMapDataModule(ID3D11Device* device, HWND hwnd);

	void setModuleParamaters(ID3D11DeviceContext* deviceContext,
		const XMFLOAT4& playerIconColour,
		float playerIconRadius,
		const XMMATRIX& orthographicViewMatrix,
		const XMFLOAT2& projectedScreenPos);

	void setResources(D3D11_SHADER_VERSION_TYPE shaderType, ID3D11DeviceContext* deviceContext, size_t startingRegister);

private:
	ComPtr<ID3D11Buffer> miniMapDataBuffer;
};