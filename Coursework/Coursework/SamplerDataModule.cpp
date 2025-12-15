#include "SamplerDataModule.h" {
	
SamplerDataModule::SamplerDataModule(ID3D11Device* device, HWND hwnd,
	D3D11_TEXTURE_ADDRESS_MODE textureMode,
	D3D11_FILTER filter,
	UINT maxAnisotropy,
	D3D11_COMPARISON_FUNC compFunc,
	const float* borderColor) :
	BaseShaderModule(device, hwnd),
	textureMode(textureMode),
	filter(filter),
	maxAnisotropy(maxAnisotropy),
	compFunc(compFunc) {
	D3D11_SAMPLER_DESC samplerDesc;

	// Create a texture sampler state description.
	samplerDesc.Filter = filter;
	samplerDesc.AddressU = textureMode;
	samplerDesc.AddressV = textureMode;
	samplerDesc.AddressW = textureMode;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = maxAnisotropy;
	samplerDesc.ComparisonFunc = compFunc;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	if (textureMode == D3D11_TEXTURE_ADDRESS_BORDER) {
		if (borderColor) {
			memcpy(samplerDesc.BorderColor, borderColor, sizeof(float) * 4);
		}
		else {
			const float defaultBorder[4] = { 1.f, 1.f, 1.f, 1.f };
			memcpy(samplerDesc.BorderColor, defaultBorder, sizeof(defaultBorder));
		}
	}
	renderer->CreateSamplerState(&samplerDesc, samplerState.GetAddressOf());
}

void SamplerDataModule::setResources(D3D11_SHADER_VERSION_TYPE shaderType, ID3D11DeviceContext* deviceContext, size_t startingRegister) {
	ID3D11SamplerState* sampleStatePtr = samplerState.Get();
	switch (shaderType) {
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_COMPUTE_SHADER:
		deviceContext->CSSetSamplers(startingRegister, 1, &sampleStatePtr);
		break;
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_DOMAIN_SHADER:
		deviceContext->DSSetSamplers(startingRegister, 1, &sampleStatePtr);
		break;
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_GEOMETRY_SHADER:
		deviceContext->GSSetSamplers(startingRegister, 1, &sampleStatePtr);
		break;
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_HULL_SHADER:
		deviceContext->HSSetSamplers(startingRegister, 1, &sampleStatePtr);
		break;
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_PIXEL_SHADER:
		deviceContext->PSSetSamplers(startingRegister, 1, &sampleStatePtr);
		break;
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_VERTEX_SHADER:
		deviceContext->VSSetSamplers(startingRegister, 1, &sampleStatePtr);
		break;
	default:
		throw std::runtime_error("Error: Shader type does not exist!");
	}
}