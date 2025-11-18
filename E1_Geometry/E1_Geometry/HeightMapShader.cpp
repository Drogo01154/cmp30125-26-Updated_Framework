#include "HeightMapShader.h"


HeightMapShader::HeightMapShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd), matrixDataModule(device, hwnd)
{
	initShader(L"heightMap_vs.cso", L"heightMap_ps.cso");
}


HeightMapShader::~HeightMapShader()
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

void HeightMapShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC mapDataBufferDesc;

	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	matrixDataModule.initModule();

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
	renderer->CreateSamplerState(&samplerDesc, sampleState.GetAddressOf());

	// Setup light buffer
	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	HRESULT lightResult = renderer->CreateBuffer(&lightBufferDesc, NULL, lightBuffer.GetAddressOf());

	if (FAILED(lightResult)) { assert(false); }
	// Setup time buffer 
	// Setup the description of the time dynammic constant buffer this is in the vertex shader.
	mapDataBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	mapDataBufferDesc.ByteWidth = sizeof(MapBufferType);
	mapDataBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	mapDataBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	mapDataBufferDesc.MiscFlags = 0;
	mapDataBufferDesc.StructureByteStride = 0;
	HRESULT timeResult = renderer->CreateBuffer(&mapDataBufferDesc, NULL, mapDataBuffer.GetAddressOf());

	if (FAILED(timeResult)) { assert(false); }

}


void HeightMapShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* heightMap, Light* light, float HeightMultiplier, int resolution)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	XMMATRIX tworld, tview, tproj;


	ID3D11Resource* resource = nullptr;
	heightMap->GetResource(&resource);

	XMFLOAT2 pixelOffset;

	//Try to cast to a 3D texture;
	ID3D11Texture2D* texture2D = nullptr;
	HRESULT result = resource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&texture2D);
	if (SUCCEEDED(result) && texture2D)
	{
		D3D11_TEXTURE2D_DESC desc;
		texture2D->GetDesc(&desc);
		pixelOffset = { 1.f / desc.Width, 1.f / desc.Height };
	}
	else {
		throw std::runtime_error("Error, could not get Width/Height of texture!");
	}

	matrixDataModule.setModuleParamaters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);

	//Additional
	//Sent time data to vertex shader
	MapBufferType* mapPtr;
	deviceContext->Map(mapDataBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	mapPtr = (MapBufferType*)mappedResource.pData;
	mapPtr->heightMultiplier = HeightMultiplier;
	mapPtr->pixelOffset = pixelOffset;
	mapPtr->resolution = resolution;
	deviceContext->Unmap(mapDataBuffer.Get(), 0);

	ID3D11Buffer* VertexBuffers[] = { mapDataBuffer.Get(), };
	deviceContext->VSSetConstantBuffers(1, 1, VertexBuffers);

	//Additional
	// Send light data to pixel shader
	LightBufferType* lightPtr;
	ID3D11Buffer* test2 = lightBuffer.Get();
	deviceContext->Map(lightBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	lightPtr = (LightBufferType*)mappedResource.pData;
	lightPtr->diffuse = light->getDiffuseColour();
	lightPtr->direction = light->getGlobalDirection();
	lightPtr->padding = 0.0f;
	deviceContext->Unmap(lightBuffer.Get(), 0);
	ID3D11Buffer* bufferPtr[] = { lightBuffer.Get(), mapDataBuffer.Get() };
	deviceContext->PSSetConstantBuffers(0, 2, bufferPtr);

	// Set shader texture resource in the pixel shader.
	ID3D11ShaderResourceView* textures[] = { texture, heightMap };
	deviceContext->PSSetShaderResources(0, 2, textures);

	deviceContext->VSSetShaderResources(0, 1, &heightMap);

	ID3D11SamplerState* samplePtr = sampleState.Get();
	deviceContext->PSSetSamplers(0, 1, &samplePtr);
	deviceContext->VSSetSamplers(0, 1, &samplePtr);
}
