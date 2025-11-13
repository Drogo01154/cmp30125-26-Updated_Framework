#pragma once

//Utilises buffer b0 for vertex shader

#include "DXF.h"

//Common module for setting up vertex data buffer
class MatrixDataModule : BaseShaderModule {
private:
	/** Default world, view, projection matrix buffer struct */
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};
public:
	MatrixDataModule(ID3D11Device* device, HWND hwnd);
	~MatrixDataModule();

	void initModule();

	void setModuleParamaters(
		ID3D11DeviceContext* deviceContext,
		const XMMATRIX& world,
		const XMMATRIX& view,
		const XMMATRIX& projection);
private: 
	ComPtr<ID3D11Buffer> matrixBuffer;
};