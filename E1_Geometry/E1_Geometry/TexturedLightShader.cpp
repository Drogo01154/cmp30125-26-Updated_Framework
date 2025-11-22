#include "TexturedLightShader.h"

TexturedLightShader::TexturedLightShader(ID3D11Device* device, HWND hwnd)
{
	initShader(L"light_vs.cso", L"texturedLight_ps.cso");
}


TexturedLightShader::~TexturedLightShader()
{
	// Release the layout.
	if (layout)
	{
		layout->Release();
		layout = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}


void TexturedLightShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);
}
void TexturedLightShader::setShaderParamaters(
	ID3D11DeviceContext* deviceContext, 
	const XMMATRIX& worldMatrix, 
	const XMMATRIX& viewMatrix, 
	const XMMATRIX& projectionMatrix, 
	std::shared_ptr<Material> material, 
	std::vector<std::shared_ptr<Light>>& lights, 
	const XMFLOAT4& ambient, 
	Camera* camera)
{
	matrixDataModule.setModuleParamaters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
	lightsDataModule.setModuleParamaters(deviceContext);

	D3D11_MAPPED_SUBRESOURCE mappedResource;

	//Additional
	// Send camera data to vertex shader
	CameraBufferType* cameraPtr;
	deviceContext->Map(cameraBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	cameraPtr = (CameraBufferType*)mappedResource.pData;
	cameraPtr->cameraPosition = camera->getGlobalPosition();
	deviceContext->Unmap(cameraBuffer.Get(), 0);
	ID3D11Buffer* cameraBufferPtr = cameraBuffer.Get();
	deviceContext->VSSetConstantBuffers(1, 1, &cameraBufferPtr);

	// Set shader texture resource in the pixel shader.

	ID3D11ShaderResourceView* texturePtr = material->texture->texture.Get();
	deviceContext->PSSetShaderResources(1, 1, &texturePtr);

}
