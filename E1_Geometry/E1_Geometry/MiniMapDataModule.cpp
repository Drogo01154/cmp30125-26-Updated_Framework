#include "MiniMapDataModule.h"


MiniMapDataModule::MiniMapDataModule(ID3D11Device* device, HWND hwnd) : BaseShaderModule(device, hwnd) {
	initModule();
}

void MiniMapDataModule::initModule() {
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC miniMapBufferDesc;

	// Setup the description of the dynamic camera constant buffer that is used in the vertex shader
	miniMapBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	miniMapBufferDesc.ByteWidth = sizeof(miniMapDataType);
	miniMapBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	miniMapBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	miniMapBufferDesc.MiscFlags = 0;
	miniMapBufferDesc.StructureByteStride = 0;
	HRESULT result = renderer->CreateBuffer(&miniMapBufferDesc, NULL, miniMapDataBuffer.GetAddressOf());
	if (FAILED(result))
	{
		assert(false);
	}
}

void MiniMapDataModule::setModuleParamaters(ID3D11DeviceContext* deviceContext,
	const XMFLOAT4& playerIconColour,
	float playerIconRadius,
	const XMMATRIX& orthographicViewMatrix,
	const XMFLOAT2& projectedScreenPos) 
{

	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	miniMapDataType* miniMapPtr;
	deviceContext->Map(miniMapDataBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	miniMapPtr = (miniMapDataType*)mappedResource.pData;
	miniMapPtr->playerIconColour = playerIconColour;
	miniMapPtr->playerIconRadius = playerIconRadius;
	miniMapPtr->projectedScreenPos = projectedScreenPos;
	deviceContext->Unmap(miniMapDataBuffer.Get(), 0);


}

void MiniMapDataModule::setResources(D3D11_SHADER_VERSION_TYPE shaderType, ID3D11DeviceContext * deviceContext, size_t startingRegister)
{
	if (shaderType != D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_PIXEL_SHADER) {
		throw std::runtime_error("Error: module must be applied to pixel shader!");
	}
	ID3D11Buffer* miniMapBufferPtr = miniMapDataBuffer.Get();
	deviceContext->PSSetConstantBuffers(startingRegister, 1, &miniMapBufferPtr);
}