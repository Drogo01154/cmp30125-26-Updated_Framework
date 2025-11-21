#include "ManipulationShader.h"

ManipulationShader::ManipulationShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd), matrixDataModule(device, hwnd)
{
	initShader(L"manipulation_vs.cso", L"manipulation_ps.cso");
}


ManipulationShader::~ManipulationShader()
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

void ManipulationShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC timeBufferDesc;

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
	timeBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	timeBufferDesc.ByteWidth = sizeof(TimeBufferType);
	timeBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	timeBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	timeBufferDesc.MiscFlags = 0;
	timeBufferDesc.StructureByteStride = 0;
	HRESULT timeResult = renderer->CreateBuffer(&timeBufferDesc, NULL, timeBuffer.GetAddressOf());

	if (FAILED(timeResult)) { assert(false); }

}


void ManipulationShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, Light* light, float time, float speed, float amplitude, float frequency)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	matrixDataModule.setModuleParamaters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
	
	//Additional
	//Sent time data to vertex shader
	TimeBufferType* timePtr;
	deviceContext->Map(timeBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	timePtr = (TimeBufferType*)mappedResource.pData;
	timePtr->time = time;
	timePtr->speed = speed;
	timePtr->amplitude = amplitude;
	timePtr->frequency = frequency;
	deviceContext->Unmap(timeBuffer.Get(), 0);

	ID3D11Buffer* VertexBuffers[] = { timeBuffer.Get() };
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
	ID3D11Buffer* bufferPtr = lightBuffer.Get();
	deviceContext->PSSetConstantBuffers(0, 1, &bufferPtr);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);

	ID3D11SamplerState* samplePtr = sampleState.Get();
	deviceContext->PSSetSamplers(0, 1, &samplePtr);
}
