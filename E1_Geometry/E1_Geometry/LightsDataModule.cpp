#include "LightsDataModule.h"

LightsDataModule::LightsDataModule(ID3D11Device* device, HWND hwnd, InstanceManager* instanceManager) : BaseShaderModule(device, hwnd), instanceManager(instanceManager) {
	lightBufferSRV = nullptr;
	lightsBuffer = nullptr;
	maxLights = 10;
	setLightsBuffer();
}

void LightsDataModule::setLightsBuffer() {
	// Relase the Light Shader Resource View
	lightBufferSRV.Reset();
	// Release the light constant buffer.
	lightsBuffer.Reset();

	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

	// Setup light buffer
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType) * maxLights;
	lightBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	lightBufferDesc.CPUAccessFlags = (lightBufferDesc.Usage == D3D11_USAGE_DYNAMIC) ? D3D11_CPU_ACCESS_WRITE : 0;
	lightBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	lightBufferDesc.StructureByteStride = sizeof(LightBufferType);
	renderer->CreateBuffer(&lightBufferDesc, NULL, lightsBuffer.GetAddressOf());

	srvDesc.Format = DXGI_FORMAT_UNKNOWN; // Required for structured buffers
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = maxLights;

	renderer->CreateShaderResourceView(lightsBuffer.Get(), &srvDesc, lightBufferSRV.GetAddressOf());
}

void LightsDataModule::setModuleParamaters(ID3D11DeviceContext* deviceContext, const std::vector<int>* indexes) {
	
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	int numLights = instanceManager->getNumberOfLights();

	if (numLights > maxLights * expandThreshold) {
		maxLights *= 2;
		setLightsBuffer();
	}
	else if ((maxLights > 10) && numLights < maxLights * shrinkThreshold) {
		maxLights /= 2;
		setLightsBuffer();
	}

	// Send light data to pixel shader
	LightBufferType* lightPtr;
	deviceContext->Map(lightsBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	lightPtr = (LightBufferType*)mappedResource.pData;

	int i = 0;
	instanceManager->forEachLight([&](const size_t& ID, Light* light) {
		lightPtr[i].attenuation = light->getAttenuation();
		lightPtr[i].position = light->getGlobalPosition();
		lightPtr[i].direction = light->getGlobalDirection();
		lightPtr[i].innerCone = cos(light->getInnerCone());
		lightPtr[i].outerCone = cos(light->getOuterCone());
		lightPtr[i].diffuse = light->getDiffuseColour();
		lightPtr[i].type = static_cast<int>(light->getType());
		lightPtr[i].constBias = light->getSlopeBias();
		lightPtr[i].slopeBias = light->getSlopeBias();
		if(indexes != nullptr) { lightPtr[i].index = (*indexes)[i]; }
		i++;
		});

	deviceContext->Unmap(lightsBuffer.Get(), 0);
}

void LightsDataModule::setResources(D3D11_SHADER_VERSION_TYPE shaderType, ID3D11DeviceContext* deviceContext, size_t startingRegister) {

	ID3D11ShaderResourceView* lightBufferSRVPtr = lightBufferSRV.Get();
	switch (shaderType) {
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_COMPUTE_SHADER:
		deviceContext->CSSetShaderResources(startingRegister, 1, &lightBufferSRVPtr);
		break;
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_DOMAIN_SHADER:
		deviceContext->DSSetShaderResources(startingRegister, 1, &lightBufferSRVPtr);
		break;
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_GEOMETRY_SHADER:
		deviceContext->GSSetShaderResources(startingRegister, 1, &lightBufferSRVPtr);
		break;
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_HULL_SHADER:
		deviceContext->HSSetShaderResources(startingRegister, 1, &lightBufferSRVPtr);
		break;
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_PIXEL_SHADER:
		deviceContext->PSSetShaderResources(startingRegister, 1, &lightBufferSRVPtr);
		break;
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_VERTEX_SHADER:
		deviceContext->VSSetShaderResources(startingRegister, 1, &lightBufferSRVPtr);
		break;
	default:
		throw std::runtime_error("Error: Shader type does not exist!");
	}
}