#include "LightTextureShader.h"

LightTextureShader::LightTextureShader(ID3D11Device* device, TextureManager* textureManager, HWND hwnd) : BaseShader(device, hwnd), matrixDataModule(device, hwnd), lightsDataModule(device, hwnd), textureManager(textureManager)
{
	initShader(L"light_vs.cso", L"lightTexture_ps.cso");
}


LightTextureShader::~LightTextureShader()
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


void LightTextureShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC cameraBufferDesc;

	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);


	// Setup the description of the dynamic camera constant buffer that is used in the vertex shader
	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;
	HRESULT result = renderer->CreateBuffer(&cameraBufferDesc, NULL, &cameraBuffer);
	if (FAILED(result))
	{
		assert(false);
	}

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	renderer->CreateSamplerState(&samplerDesc, &sampleState);

	lightsDataModule.initModule();
}
void LightTextureShader::setShaderParamaters(
	ID3D11DeviceContext* deviceContext, 
	const XMMATRIX& worldMatrix, 
	const XMMATRIX& viewMatrix, 
	const XMMATRIX& projectionMatrix, 
	std::shared_ptr<Material> material, 
	std::vector<std::shared_ptr<Light>>& lights, 
	const XMFLOAT4& ambient, 
	Camera* camera)
{
	lightsDataModule.setModuleParamaters(deviceContext, material, lights, ambient);

	D3D11_MAPPED_SUBRESOURCE mappedResource;

	//Additional
	// Send camera data to vertex shader
	CameraBufferType* cameraPtr;
	deviceContext->Map(cameraBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	cameraPtr = (CameraBufferType*)mappedResource.pData;
	cameraPtr->cameraPosition = camera->getPosition();
	deviceContext->Unmap(cameraBuffer.Get(), 0);
	deviceContext->VSSetConstantBuffers(1, 1, cameraBuffer.GetAddressOf());

	// Set shader texture resource in the pixel shader.

	ID3D11ShaderResourceView* texturePtr = textureManager->getTexture(material->texture);
	deviceContext->PSSetShaderResources(1, 1, &texturePtr);
	deviceContext->PSSetSamplers(0, 1, &sampleState);
}
