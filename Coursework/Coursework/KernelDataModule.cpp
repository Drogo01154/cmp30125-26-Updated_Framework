#include "KernelDataModule.h"

KernelDataModule::KernelDataModule(ID3D11Device* device, HWND hwnd, int screenWidth, int screenHeight) {
	texelWidth = 1.f / screenWidth;
	texelHeight = 1.f / screenHeight;
	D3D11_BUFFER_DESC screenDataBufferDesc;
	// Setup the description of the dynamic camera constant buffer that is used in the vertex shader
	screenDataBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	screenDataBufferDesc.ByteWidth = sizeof(screenDataBufferType);
	screenDataBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	screenDataBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	screenDataBufferDesc.MiscFlags = 0;
	screenDataBufferDesc.StructureByteStride = 0;
	HRESULT result = renderer->CreateBuffer(&screenDataBufferDesc, NULL, screenDataBuffer.GetAddressOf());
	if (FAILED(result))
	{
		assert(false);
	}
}

void KernelDataModule::setModuleParamaters(ID3D11DeviceContext* deviceContext) {
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	
	screenDataBufferType* screenDataPtr;
	deviceContext->Map(screenDataBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	screenDataPtr = (screenDataBufferType*)mappedResource.pData;
	screenDataPtr->texelWidth = texelWidth;
	screenDataPtr->texelHeight = texelHeight;

	deviceContext->Unmap(screenDataBuffer.Get(), 0);
}

void KernelDataModule::setResources(D3D11_SHADER_VERSION_TYPE shaderType, ID3D11DeviceContext* deviceContext, size_t startingRegister) {
	if (shaderType != D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_PIXEL_SHADER) {
		throw std::runtime_error("Error: Must be a pixel shader!");
	}
	ID3D11Buffer* screenDataBufferPtr = screenDataBuffer.Get();
	deviceContext->PSSetConstantBuffers(startingRegister, 1, &screenDataBufferPtr);
}