#include "CameraDataModule.h"

CameraDataModule::CameraDataModule(ID3D11Device* device, HWND hwnd, InstanceManager* instanceManager, SceneGraph* sceneGraph) :
BaseShaderModule(device, hwnd),
instanceManager(instanceManager),
sceneGraph(sceneGraph){

}

CameraDataModule::~CameraDataModule() {
	
}

void CameraDataModule::initModule() {
	D3D11_BUFFER_DESC cameraDataBufferDesc;
	// Setup the description of the dynamic camera data buffer that can be used in different shader stages
	cameraDataBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraDataBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraDataBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraDataBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraDataBufferDesc.MiscFlags = 0;
	cameraDataBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&cameraDataBufferDesc, NULL, cameraDataBuffer.GetAddressOf());
}

void CameraDataModule::setModuleParamaters(ID3D11DeviceContext* deviceContext) {
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	// Send world data to buffer
	CameraBufferType* cameraPtr;

	deviceContext->Map(cameraDataBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	cameraPtr = (CameraBufferType*)mappedResource.pData;
	cameraPtr->ambient = sceneGraph->getAmbientLight();
	cameraPtr->cameraPosition = instanceManager->getActiveCamera()->camera->getGlobalPosition();
	cameraPtr->numLights = instanceManager->getNumberOfLights();

	deviceContext->Unmap(cameraDataBuffer.Get(), 0);
}

void CameraDataModule::setResources(D3D11_SHADER_VERSION_TYPE shaderType, ID3D11DeviceContext* deviceContext, size_t startingRegister) {
	
	ID3D11Buffer* buffers[] = { cameraDataBuffer.Get() };
	switch (shaderType) {
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_COMPUTE_SHADER:
		deviceContext->CSSetConstantBuffers(startingRegister, 1, buffers);
		break;
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_DOMAIN_SHADER:
		deviceContext->DSSetConstantBuffers(startingRegister, 1, buffers);
		break;
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_GEOMETRY_SHADER:
		deviceContext->GSSetConstantBuffers(startingRegister, 1, buffers);
		break;
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_HULL_SHADER:
		deviceContext->HSSetConstantBuffers(startingRegister, 1, buffers);
		break;
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_PIXEL_SHADER:
		deviceContext->PSSetConstantBuffers(startingRegister, 1, buffers);
		break;
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_VERTEX_SHADER:
		deviceContext->VSSetConstantBuffers(startingRegister, 1, buffers);
		break;
	default:
		throw std::runtime_error("Error: Shader type does not exist!");
	}
}
