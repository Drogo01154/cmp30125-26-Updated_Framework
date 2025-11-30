#pragma once
#include "DXF.h"

class ShadowMapDataModule : public BaseShaderModule {

	ShadowMapDataModule(ID3D11Device* device, HWND hwnd);

	void setMatrixBuffer();
	void setModuleParamaters(ID3D11DeviceContext* deviceContext, const std::vector<XMMATRIX>* matrixes, ID3D11ShaderResourceView* ShadowMapArray, ID3D11ShaderResourceView* CubeMapArray);
	void setResources(D3D11_SHADER_VERSION_TYPE shaderType, ID3D11DeviceContext* deviceContext, size_t startingRegister);

private:

	ComPtr<ID3D11Buffer> viewProjMatricesBuffer;
	ComPtr<ID3D11ShaderResourceView> viewProjMatrixBufferSRV;
	
	//Ptrs to array SRVs from respective classes
	ID3D11ShaderResourceView* ShadowMapArray;
	ID3D11ShaderResourceView* CubeMapArray;

	int numLights;
	int maxLights;

	static constexpr float expandThreshold = 1.0f;   // 100% of capacity
	static constexpr float shrinkThreshold = 0.25f;  // 25% of capacity
};