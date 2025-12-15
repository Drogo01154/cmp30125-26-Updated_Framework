#include "LightDepthDataBuffer.h"

LightDepthDataBuffer::LightDepthDataBuffer(ID3D11Device* device, HWND hwnd) : BaseShaderModule(device, hwnd) {
	D3D11_BUFFER_DESC DepthDataBufferDesc;

	// Setup the description of the map data constant buffer

	DepthDataBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	DepthDataBufferDesc.ByteWidth = sizeof(DepthDataBufferType);
	DepthDataBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	DepthDataBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	DepthDataBufferDesc.MiscFlags = 0;
	DepthDataBufferDesc.StructureByteStride = 0;
	HRESULT timeResult = renderer->CreateBuffer(&DepthDataBufferDesc, NULL, DepthDataBuffer.GetAddressOf());

	if (FAILED(timeResult)) { assert(false); }
}

void LightDepthDataBuffer::setModuleParamaters(ID3D11DeviceContext* deviceContext, Light* light) {
	DepthDataBufferType* depthDataPtr;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	deviceContext->Map(DepthDataBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	depthDataPtr = (DepthDataBufferType*)mappedResource.pData;
	lightTypes lightType = light->getType();
	if (lightType == lightTypes::point) {
		depthDataPtr->farPlane = light->getFar();
		depthDataPtr->lightPosition = light->getGlobalPosition();
	}
	depthDataPtr->lightType = static_cast<int>(lightType);
	deviceContext->Unmap(DepthDataBuffer.Get(), 0);
}

void LightDepthDataBuffer::setResources(D3D11_SHADER_VERSION_TYPE shaderType, ID3D11DeviceContext* deviceContext, size_t startingRegister) {
	ID3D11Buffer* pointLigthBufferPtr = DepthDataBuffer.Get();

	switch (shaderType) {
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_COMPUTE_SHADER:
		deviceContext->CSSetConstantBuffers(startingRegister, 1, &pointLigthBufferPtr);
		break;
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_DOMAIN_SHADER:
		deviceContext->DSSetConstantBuffers(startingRegister, 1, &pointLigthBufferPtr);
		break;
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_GEOMETRY_SHADER:
		deviceContext->GSSetConstantBuffers(startingRegister, 1, &pointLigthBufferPtr);
		break;
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_HULL_SHADER:
		deviceContext->HSSetConstantBuffers(startingRegister, 1, &pointLigthBufferPtr);
		break;
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_PIXEL_SHADER:
		deviceContext->PSSetConstantBuffers(startingRegister, 1, &pointLigthBufferPtr);
		break;
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_VERTEX_SHADER:
		deviceContext->VSSetConstantBuffers(startingRegister, 1, &pointLigthBufferPtr);
		break;
	default:
		throw std::runtime_error("Error: Shader type does not exist!");
	}
}