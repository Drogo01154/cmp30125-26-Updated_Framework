#include "LightShader.h"

LightShader::LightShader(ID3D11Device* device, TextureManager* textureManager, HWND hwnd) : BaseShader(device, hwnd), textureManager(textureManager)
{
	lightBufferSRV = nullptr;
	lightBuffer = nullptr;
	maxLights = 10;

	initShader(L"light_vs.cso", L"light_ps.cso");
}


LightShader::~LightShader()
{
	// Release the sampler state.
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}

	// Release the matrix constant buffer.
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}

	// Release the layout.
	if (layout)
	{
		layout->Release();
		layout = 0;
	}

	// Release the light constant buffer.
	if (lightBuffer)
	{
		lightBuffer->Release();
		lightBuffer = 0;
	}

	// Relase the Light Shader Resource View
	if (lightBufferSRV)
	{
		lightBufferSRV->Release();
		lightBufferSRV = 0;
	}

	if (worldDataBuffer)
	{
		worldDataBuffer->Release();
		worldDataBuffer = 0;
	}

	if (cameraBuffer)
	{
		cameraBuffer->Release();
		cameraBuffer = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}

void LightShader::setLightBuffer() {
	// Relase the Light Shader Resource View
	if (lightBufferSRV)
	{
		lightBufferSRV->Release();
		lightBufferSRV = 0;
	}

	// Release the light constant buffer.
	if (lightBuffer)
	{
		lightBuffer->Release();
		lightBuffer = 0;
	}

	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

	// Setup light buffer
	// Setup the description of the light dynamic structured buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType) * maxLights;
	lightBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	lightBufferDesc.CPUAccessFlags = (lightBufferDesc.Usage == D3D11_USAGE_DYNAMIC) ? D3D11_CPU_ACCESS_WRITE : 0;
	lightBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	lightBufferDesc.StructureByteStride = sizeof(LightBufferType);
	renderer->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);

	srvDesc.Format = DXGI_FORMAT_UNKNOWN; // Required for structured buffers
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = maxLights;

	renderer->CreateShaderResourceView(lightBuffer, &srvDesc, &lightBufferSRV);
}

void LightShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC worldDataBufferDesc;
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

	// Setup the description of the dynamic world data constant buffer that is used in the pixel shader. 
	worldDataBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	worldDataBufferDesc.ByteWidth = sizeof(WorldBufferType);
	worldDataBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	worldDataBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	worldDataBufferDesc.MiscFlags = 0;
	worldDataBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&worldDataBufferDesc, NULL, &worldDataBuffer);
	


	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

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

	setLightBuffer();
}
void LightShader::setShaderParamaters(
	ID3D11DeviceContext* deviceContext, 
	const XMMATRIX& worldMatrix, 
	const XMMATRIX& viewMatrix, 
	const XMMATRIX& projectionMatrix, 
	std::shared_ptr<Material> material, 
	std::vector<std::shared_ptr<Light>>& lights, 
	const XMFLOAT4& ambient, 
	Camera* camera)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;

	XMMATRIX tworld, tview, tproj;

	int numLights = lights.size();

	if (numLights > maxLights * expandThreshold) {
		maxLights *= 2;
		setLightBuffer();
	}
	else if ((maxLights > 10) && numLights < maxLights * shrinkThreshold) {
		maxLights /= 2;
		setLightBuffer();
	}

	// Transpose the matrices to prepare them for the shader.
	tworld = XMMatrixTranspose(worldMatrix);
	tview = XMMatrixTranspose(viewMatrix);
	tproj = XMMatrixTranspose(projectionMatrix);
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);

	//Additional
	// Send camera data to vertex shader
	CameraBufferType* cameraPtr;
	deviceContext->Map(cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	cameraPtr = (CameraBufferType*)mappedResource.pData;
	cameraPtr->cameraPosition = camera->getPosition();
	deviceContext->Unmap(cameraBuffer, 0);
	deviceContext->VSSetConstantBuffers(1, 1, &cameraBuffer);

	// Send world data to pixel shader
	WorldBufferType* worldPtr;
	deviceContext->Map(worldDataBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	worldPtr = (WorldBufferType*)mappedResource.pData;
	worldPtr->ambientLight = ambient;
	worldPtr->numberOfLights = numLights;
	worldPtr->specular = material->specularColour;
	worldPtr->specularPower = material->specularPower;

	deviceContext->Unmap(worldDataBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &worldDataBuffer);
	
	// Send light data to pixel shader
	LightBufferType* lightPtr;
	deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	
	lightPtr = (LightBufferType*)mappedResource.pData;

	for (int i = 0; i < maxLights; i++)
	{
		if (i < lights.size())
		{
			lightPtr[i].attenuation = lights[i]->getAttenuation();
			lightPtr[i].position = lights[i]->getPosition();
			lightPtr[i].direction = lights[i]->getDirectionNormalized();
			lightPtr[i].innerCone = lights[i]->getInnerCone();
			lightPtr[i].outerCone = lights[i]->getOuterCone();
			lightPtr[i].diffuse = lights[i]->getDiffuseColour();
			lightPtr[i].type = static_cast<int>(lights[i]->getType());
		}
		else {
			// Zero out the remaining lights
			lightPtr[i] = {};
		}
		
		
	}

	deviceContext->Unmap(lightBuffer, 0);
	deviceContext->PSSetShaderResources(1, 1, &lightBufferSRV);

	// Set shader texture resource in the pixel shader.

	ID3D11ShaderResourceView* texturePtr = textureManager->getTexture(material->texture);
	deviceContext->PSSetShaderResources(0, 1, &texturePtr);
	deviceContext->PSSetSamplers(0, 1, &sampleState);
}
