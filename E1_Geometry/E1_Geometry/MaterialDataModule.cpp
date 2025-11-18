#include "MaterialDataModule.h"

MaterialDataModule::MaterialDataModule(ID3D11Device* device, HWND hwnd, InstanceManager* instanceManager, SceneGraph* sceneGraph) : 
	BaseShaderModule(device, hwnd), 
	instanceManager(instanceManager),
	sceneGraph(sceneGraph){
	sceneDataBuffer = nullptr;
}


MaterialDataModule::~MaterialDataModule() {

}

void MaterialDataModule::initModule() {
	D3D11_BUFFER_DESC sceneDataBufferDesc;

	// Setup the description of the dynamic world data constant buffer that is used in the pixel shader. 
	sceneDataBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	sceneDataBufferDesc.ByteWidth = sizeof(SceneBufferType);
	sceneDataBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	sceneDataBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	sceneDataBufferDesc.MiscFlags = 0;
	sceneDataBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&sceneDataBufferDesc, NULL, sceneDataBuffer.GetAddressOf());
}

void MaterialDataModule::setModuleParamaters(ID3D11DeviceContext* deviceContext, std::shared_ptr<Material> material) {

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	// Send world data to pixel shader
	SceneBufferType* scenePtr;
	deviceContext->Map(sceneDataBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	scenePtr = (SceneBufferType*)mappedResource.pData;
	scenePtr->baseColour = material->baseColour;
	scenePtr->ambientLight = sceneGraph->getAmbientLight();
	scenePtr->numberOfLights = instanceManager->getNumberOfLights();
	scenePtr->specular = material->specularColour;
	scenePtr->specularPower = material->specularPower;

	deviceContext->Unmap(sceneDataBuffer.Get(), 0);
}

void MaterialDataModule::setResources(ID3D11DeviceContext* deviceContext) {
	ID3D11Buffer* sceneDataBufferPtr = sceneDataBuffer.Get();
	deviceContext->PSSetConstantBuffers(0, 1, &sceneDataBufferPtr);
}