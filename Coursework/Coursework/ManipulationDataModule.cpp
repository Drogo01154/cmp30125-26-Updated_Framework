#include "ManipulationDataModule.h"

#pragma once

ManipulationDataModule::ManipulationDataModule(ID3D11Device* device, HWND hwnd) : BaseShaderModule(device, hwnd) {
	D3D11_BUFFER_DESC timeBufferDesc;

	// Setup time buffer 
	// Setup the description of the time dynammic constant buffer this is in the vertex shader.
	timeBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	timeBufferDesc.ByteWidth = sizeof(TimeBufferType);
	timeBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	timeBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	timeBufferDesc.MiscFlags = 0;
	timeBufferDesc.StructureByteStride = 0;
	HRESULT timeResult = renderer->CreateBuffer(&timeBufferDesc, NULL, timeBuffer.GetAddressOf());

	if (FAILED(timeResult)) { assert(false); }
}

void ManipulationDataModule::setModuleParamaters(ID3D11DeviceContext* deviceContext, float time, float speed, float amplitude, float frequency) {
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	
	TimeBufferType* timePtr;
	deviceContext->Map(timeBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	timePtr = (TimeBufferType*)mappedResource.pData;
	timePtr->time = time;
	timePtr->speed = speed;
	timePtr->amplitude = amplitude;
	timePtr->frequency = frequency;
	deviceContext->Unmap(timeBuffer.Get(), 0);
}

void ManipulationDataModule::setResources(D3D11_SHADER_VERSION_TYPE shaderType, ID3D11DeviceContext* deviceContext, size_t startingRegister) {
	if (shaderType != D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_VERTEX_SHADER) {
		throw std::runtime_error("Error: Must be a vertex shader");
	}
	ID3D11Buffer* VertexBuffers[] = { timeBuffer.Get() };
	deviceContext->VSSetConstantBuffers(startingRegister, 1, VertexBuffers);
}
/*
#include "DXF.h"
#include "SceneGraph.h"

using namespace std;
using namespace DirectX;

class ManipulationDataModule : BaseShaderModule {
private:

	struct TimeBufferType
	{
		float time;
		float speed;
		float amplitude;
		float frequency;
	};

public:
	ManipulationDataModule();
	~ManipulationDataModule();

	void setModuleParamaters(ID3D11DeviceContext* deviceContext, Material* material);

	void setResources(D3D11_SHADER_VERSION_TYPE shaderType, ID3D11DeviceContext* deviceContext, size_t startingRegister);

	void initModule();

private:
	ComPtr<ID3D11Buffer> timeBuffer;
};
*/