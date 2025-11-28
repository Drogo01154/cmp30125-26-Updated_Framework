#pragma once

#include "DXF.h"
#include "SceneGraph.h"

using namespace std;
using namespace DirectX;

class ManipulationDataModule : public BaseShaderModule {
private:

	struct TimeBufferType
	{
		float time;
		float speed;
		float amplitude;
		float frequency;
	};

public:
	ManipulationDataModule(ID3D11Device* device, HWND hwnd);

	void initModule();
	void setModuleParamaters(ID3D11DeviceContext* deviceContext, float time, float speed, float amplitude, float frequency);
	void setResources(D3D11_SHADER_VERSION_TYPE shaderType, ID3D11DeviceContext* deviceContext, size_t startingRegister);

private:
	ComPtr<ID3D11Buffer> timeBuffer;
};