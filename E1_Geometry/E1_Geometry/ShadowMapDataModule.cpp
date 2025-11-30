#include "ShadowMapDataModule.h"

ShadowMapDataModule::ShadowMapDataModule(ID3D11Device* device, HWND hwnd) : BaseShaderModule(device, hwnd) {
	setMatrixBuffer();
}

void ShadowMapDataModule::setMatrixBuffer() {
	D3D11_BUFFER_DESC viewProjBufferDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	// Setup light buffer
	viewProjBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	viewProjBufferDesc.ByteWidth = sizeof(XMMATRIX) * maxLights;
	viewProjBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	viewProjBufferDesc.CPUAccessFlags = (viewProjBufferDesc.Usage == D3D11_USAGE_DYNAMIC) ? D3D11_CPU_ACCESS_WRITE : 0;
	viewProjBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	viewProjBufferDesc.StructureByteStride = sizeof(XMMATRIX);
	renderer->CreateBuffer(&viewProjBufferDesc, NULL, viewProjMatricesBuffer.GetAddressOf());

	srvDesc.Format = DXGI_FORMAT_UNKNOWN; // Required for structured buffers
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = maxLights;

	renderer->CreateShaderResourceView(viewProjMatricesBuffer.Get(), &srvDesc, viewProjMatrixBufferSRV.GetAddressOf());
}

void ShadowMapDataModule::setModuleParamaters(ID3D11DeviceContext* deviceContext, const std::vector<XMMATRIX>* matrixes, ID3D11ShaderResourceView* ShadowMapArray, ID3D11ShaderResourceView* CubeMapArray) {
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	int numLights = matrixes->size();

	if (numLights > maxLights * expandThreshold) {
		maxLights *= 2;
		setMatrixBuffer();
	}
	else if ((maxLights > 10) && numLights < maxLights * shrinkThreshold) {
		maxLights /= 2;
		setMatrixBuffer();
	}

	// Send light data to pixel shader
	XMMATRIX* matrixPtr;

	deviceContext->Map(viewProjMatricesBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	matrixPtr = (XMMATRIX*)mappedResource.pData;

	for (int i = 0; i < matrixes->size(); ++i) {
		matrixPtr[i] = XMMatrixTranspose((*matrixes)[i]);
	}

	deviceContext->Unmap(viewProjMatricesBuffer.Get(), 0);
	this->ShadowMapArray = ShadowMapArray;
	this->CubeMapArray = CubeMapArray;
}

void ShadowMapDataModule::setResources(D3D11_SHADER_VERSION_TYPE shaderType, ID3D11DeviceContext* deviceContext, size_t startingRegister) {
	ID3D11ShaderResourceView* ShadowMapResources[] = { viewProjMatrixBufferSRV.Get(), ShadowMapArray, CubeMapArray };

	switch (shaderType) {
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_COMPUTE_SHADER:
		deviceContext->CSSetShaderResources(startingRegister, ARRAYSIZE(ShadowMapResources), ShadowMapResources);
		break;
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_DOMAIN_SHADER:
		deviceContext->DSSetShaderResources(startingRegister, ARRAYSIZE(ShadowMapResources), ShadowMapResources);
		break;
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_GEOMETRY_SHADER:
		deviceContext->GSSetShaderResources(startingRegister, ARRAYSIZE(ShadowMapResources), ShadowMapResources);
		break;
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_HULL_SHADER:
		deviceContext->HSSetShaderResources(startingRegister, ARRAYSIZE(ShadowMapResources), ShadowMapResources);
		break;
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_PIXEL_SHADER:
		deviceContext->PSSetShaderResources(startingRegister, ARRAYSIZE(ShadowMapResources), ShadowMapResources);
		break;
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_VERTEX_SHADER:
		deviceContext->VSSetShaderResources(startingRegister, ARRAYSIZE(ShadowMapResources), ShadowMapResources);
		break;
	default:
		throw std::runtime_error("Error: Shader type does not exist!");
	}
}