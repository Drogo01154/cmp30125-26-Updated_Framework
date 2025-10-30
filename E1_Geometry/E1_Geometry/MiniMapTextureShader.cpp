#include "MiniMapTextureShader.h"

MiniMapTextureShader::MiniMapTextureShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd), matrixDataModule(device, hwnd)
{
	initShader(L"texture_vs.cso", L"textureMiniMap_ps.cso");
}


MiniMapTextureShader::~MiniMapTextureShader()
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


void MiniMapTextureShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC miniMapBufferDesc;

	// Setup the description of the dynamic camera constant buffer that is used in the vertex shader
	miniMapBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	miniMapBufferDesc.ByteWidth = sizeof(miniMapDataType);
	miniMapBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	miniMapBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	miniMapBufferDesc.MiscFlags = 0;
	miniMapBufferDesc.StructureByteStride = 0;
	HRESULT result = renderer->CreateBuffer(&miniMapBufferDesc, NULL, miniMapBuffer.GetAddressOf());
	if (FAILED(result))
	{
		assert(false);
	}


	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	matrixDataModule.initModule();


	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	renderer->CreateSamplerState(&samplerDesc, sampleState.GetAddressOf());

}


void MiniMapTextureShader::setShaderParameters(ID3D11DeviceContext* deviceContext,
	const XMMATRIX& worldMatrix,
	const XMMATRIX& viewMatrix,
	const XMMATRIX& projectionMatrix,
	ID3D11ShaderResourceView* texture,
	const XMFLOAT3& cameraScreenPosition,
	const XMFLOAT4& playerIconColour,
	const XMFLOAT3& greyScaleValues,
	float playerIconRadius)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	XMMATRIX tworld, tview, tproj;

	// Transpose the matrices to prepare them for the shader.
	tworld = XMMatrixTranspose(worldMatrix);
	tview = XMMatrixTranspose(viewMatrix);
	tproj = XMMatrixTranspose(projectionMatrix);

	matrixDataModule.setModuleParamaters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);

	miniMapDataType* miniMapPtr;
	deviceContext->Map(miniMapBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	miniMapPtr = (miniMapDataType*)mappedResource.pData;
	miniMapPtr->playerIconColour = playerIconColour;
	miniMapPtr->greyScaleValues = greyScaleValues;
	miniMapPtr->playerIconRadius = playerIconRadius;
	miniMapPtr->cameraPos = cameraScreenPosition;

	deviceContext->Unmap(miniMapBuffer.Get(), 0);

	ID3D11Buffer* miniMapBufferPtr = miniMapBuffer.Get();
	deviceContext->PSSetConstantBuffers(0, 1, &miniMapBufferPtr);

	// Set shader texture and sampler resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);
	ID3D11SamplerState* sampleStatePtr = sampleState.Get();
	deviceContext->PSSetSamplers(0, 1, &sampleStatePtr);
}