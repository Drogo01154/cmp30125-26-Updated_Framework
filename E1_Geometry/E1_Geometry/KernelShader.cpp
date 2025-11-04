#include "KernelShader.h"

KernelShader::KernelShader(ID3D11Device* device, HWND hwnd, KernelEffects effect, int screenWidth, int screenHeight) : BaseShader(device, hwnd), matrixDataModule(device, hwnd)
{
	texelWidth = 1.f / screenWidth;
	texelHeight = 1.f / screenHeight;
	switch (effect) {
	case KernelEffects::BOX_BLUR:
		initShader(L"texture_vs.cso", L"BoxBlur_ps.cso");
		break;
	case KernelEffects::EDGE_DETECTION:
		initShader(L"texture_vs.cso", L"EdgeDetection_ps.cso");
		break;
	case KernelEffects::GAUSSIAN3X3:
		initShader(L"texture_vs.cso", L"Gaussian3X3_ps.cso");
		break;
	case KernelEffects::GAUSSIAN5X5:
		initShader(L"texture_vs.cso", L"texture_ps.cso");
		break;
	case KernelEffects::SHARPEN:
		initShader(L"texture_vs.cso", L"Gaussian5X5_ps.cso");
		break;
	}
}


KernelShader::~KernelShader()
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


void KernelShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC screenDataBufferDesc;

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
	renderer->CreateSamplerState(&samplerDesc, &sampleState);

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


void KernelShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	XMMATRIX tworld, tview, tproj;


	// Transpose the matrices to prepare them for the shader.
	tworld = XMMatrixTranspose(worldMatrix);
	tview = XMMatrixTranspose(viewMatrix);
	tproj = XMMatrixTranspose(projectionMatrix);

	matrixDataModule.setModuleParamaters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);

	// Set shader texture and sampler resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);
	ID3D11SamplerState* sampler = sampleState.Get();
	deviceContext->PSSetSamplers(0, 1, &sampler);

	screenDataBufferType* screenDataPtr;
	deviceContext->Map(screenDataBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	screenDataPtr = (screenDataBufferType*)mappedResource.pData;
	screenDataPtr->texelWidth = texelWidth;
	screenDataPtr->texelHeight = texelHeight;

	deviceContext->Unmap(screenDataBuffer.Get(), 0);

	ID3D11Buffer* screenDataBufferPtr = screenDataBuffer.Get();
	deviceContext->PSSetConstantBuffers(0, 1, &screenDataBufferPtr);
}