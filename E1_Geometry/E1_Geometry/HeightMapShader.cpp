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
	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	
}


void HeightMapShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* heightMap, Light* light, float HeightMultiplier, int resolution)
{
	

	XMMATRIX tworld, tview, tproj;


	

	//Try to cast to a 3D texture;
	

	matrixDataModule.setResources(deviceContext);

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
