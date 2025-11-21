#pragma once

#include "DXF.h"
#include "SceneGraph.h"

using namespace std;
using namespace DirectX;



class CameraDataModule : BaseShaderModule {
private:
	struct CameraBufferType
	{
		XMFLOAT3 cameraPosition;
		int numLights;
	};

public:
	CameraDataModule(ID3D11Device * device, HWND hwnd, InstanceManager* instanceManager);
	~CameraDataModule();

	void setModuleParamaters(ID3D11DeviceContext* deviceContext);

	void setResources(ID3D11DeviceContext* deviceContext, size_t );

	void initModule();

private:
	ComPtr<ID3D11Buffer> sceneDataBuffer;
	InstanceManager* instanceManager;
	SceneGraph* sceneGraph;
};