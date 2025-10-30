#include "GreyScaleTextureShader.h"

GreyScaleTextureShader::GreyScaleTextureShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd), matrixDataModule(device, hwnd)
{
	initShader(L"texture_vs.cso", L"textureGreyscale_ps.cso");
}


GreyScaleTextureShader::~GreyScaleTextureShader()
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


void GreyScaleTextureShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_SAMPLER_DESC samplerDesc;
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


void GreyScaleTextureShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, const XMFLOAT3& greyScale)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	XMMATRIX tworld, tview, tproj;


	// Transpose the matrices to prepare them for the shader.
	tworld = XMMatrixTranspose(worldMatrix);
	tview = XMMatrixTranspose(viewMatrix);
	tproj = XMMatrixTranspose(projectionMatrix);

	matrixDataModule.setModuleParamaters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);

	greyScaleBufferType* greyScalePtr;
	deviceContext->Map(greyScaleBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	greyScalePtr = (greyScaleBufferType*)mappedResource.pData;
	greyScalePtr->greyScale = greyScale;
	deviceContext->Unmap(greyScaleBuffer.Get(), 0);

	
	ID3D11Buffer* greyScaleBufferPtr = greyScaleBuffer.Get();
	deviceContext->PSSetConstantBuffers(0, 1, &greyScaleBufferPtr);


	// Set shader texture and sampler resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);
	ID3D11SamplerState* sampleStatePtr = sampleState.Get();
	deviceContext->PSSetSamplers(0, 1, &sampleStatePtr);
}