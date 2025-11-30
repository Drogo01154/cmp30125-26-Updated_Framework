#pragma once

#include "DXF.h"
#include "SceneGraph.h"

using namespace std;
using namespace DirectX;



class CameraDataModule : public BaseShaderModule {
private:
	struct CameraBufferType
	{
		XMFLOAT4 ambient;
		XMFLOAT3 cameraPosition;
		int numLights;
	};

public:
	CameraDataModule(ID3D11Device * device, HWND hwnd, InstanceManager* instanceManager, SceneGraph* sceneGraph);

	void setModuleParamaters(ID3D11DeviceContext* deviceContext);

	void setResources(D3D11_SHADER_VERSION_TYPE shaderType, ID3D11DeviceContext* deviceContext, size_t startingRegister);

private:
	ComPtr<ID3D11Buffer> cameraDataBuffer;
	InstanceManager* instanceManager;
	SceneGraph* sceneGraph;
};