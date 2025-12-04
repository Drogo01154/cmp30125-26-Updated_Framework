#include "PointLightDataModule.h"

PointLightDataModule::PointLightDataModule(ID3D11Device* device, HWND hwnd) : BaseShaderModule(device, hwnd) {
	D3D11_BUFFER_DESC PointLightBufferDesc;

	// Setup the description of the map data constant buffer

	PointLightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	PointLightBufferDesc.ByteWidth = sizeof(PointLightBufferType);
	PointLightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	PointLightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	PointLightBufferDesc.MiscFlags = 0;
	PointLightBufferDesc.StructureByteStride = 0;
	HRESULT timeResult = renderer->CreateBuffer(&PointLightBufferDesc, NULL, PointLightBuffer.GetAddressOf());

	if (FAILED(timeResult)) { assert(false); }
}

void PointLightDataModule::setModuleParamaters(ID3D11DeviceContext* deviceContext, const XMFLOAT3& lightPosition, float farPlane) {
	PointLightBufferType* pointLightPtr;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	deviceContext->Map(PointLightBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	pointLightPtr = (PointLightBufferType*)mappedResource.pData;
	pointLightPtr->lightPosition = lightPosition;
	pointLightPtr->farPlane = farPlane;
	deviceContext->Unmap(PointLightBuffer.Get(), 0);
}

void PointLightDataModule::setResources(D3D11_SHADER_VERSION_TYPE shaderType, ID3D11DeviceContext* deviceContext, size_t startingRegister) {
	ID3D11Buffer* pointLigthBufferPtr = PointLightBuffer.Get();

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