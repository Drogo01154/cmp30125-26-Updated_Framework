#include "GreyScaleDataModule.h"

GreyScaleDataModule::GreyScaleDataModule(ID3D11Device* device, HWND hwnd) {
	initModule();
}

void GreyScaleDataModule::initModule() {
	D3D11_BUFFER_DESC greyScaleBufferDesc;

	// Setup the description of the dynamic camera constant buffer that is used in the vertex shader
	greyScaleBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	greyScaleBufferDesc.ByteWidth = sizeof(greyScaleBufferType);
	greyScaleBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	greyScaleBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	greyScaleBufferDesc.MiscFlags = 0;
	greyScaleBufferDesc.StructureByteStride = 0;
	HRESULT result = renderer->CreateBuffer(&greyScaleBufferDesc, NULL, greyScaleBuffer.GetAddressOf());
	if (FAILED(result))
	{
		assert(false);
	}
}
void GreyScaleDataModule::setModuleParamaters(ID3D11DeviceContext* deviceContext, const XMFLOAT3& greyScale) {
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	greyScaleBufferType* greyScalePtr;
	deviceContext->Map(greyScaleBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	greyScalePtr = (greyScaleBufferType*)mappedResource.pData;
	greyScalePtr->greyScale = greyScale;
	deviceContext->Unmap(greyScaleBuffer.Get(), 0);
}
void GreyScaleDataModule::setResources(D3D11_SHADER_VERSION_TYPE shaderType, ID3D11DeviceContext* deviceContext, size_t startingRegister) {
	if (shaderType != D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_PIXEL_SHADER) {
		throw std::runtime_error("Error: Must be a pixel shader!");
	}
	ID3D11Buffer* greyScaleBufferPtr = greyScaleBuffer.Get();
	deviceContext->PSSetConstantBuffers(startingRegister, 1, &greyScaleBufferPtr);
}