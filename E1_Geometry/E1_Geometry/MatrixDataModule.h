#pragma once

//Utilises buffer b0 for vertex shader

#include "DXF.h"

//Common module for setting up vertex data buffer
class MatrixDataModule : public BaseShaderModule {
private:
	/** Default world, view, projection matrix buffer struct */
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};
public:
	MatrixDataModule(ID3D11Device* device, HWND hwnd, InstanceManager* instanceManager);
	~MatrixDataModule();

	void initModule();
	void setModuleParamaters(
		ID3D11DeviceContext* deviceContext,
		GeometryData* mesh, const XMMATRIX& projectionMatrix, bool orthCameraMat = false);
	void setResources(D3D11_SHADER_VERSION_TYPE shaderType, ID3D11DeviceContext* deviceContext, size_t startingRegister);

private: 
	InstanceManager* instanceManager;
	ComPtr<ID3D11Buffer> matrixBuffer;
};