#include "LightsDataModule.h"

LightsDataModule::LightsDataModule(ID3D11Device* device, HWND hwnd, InstanceManager* instanceManager) : BaseShaderModule(device, hwnd), instanceManager(instanceManager) {
	lightBufferSRV = nullptr;
	lightsBuffer = nullptr;
	maxLights = 10;
}


LightsDataModule::~LightsDataModule() {

}

void LightsDataModule::setLightsBuffer() {
	// Relase the Light Shader Resource View
	lightBufferSRV.Reset();
	// Release the light constant buffer.
	lightsBuffer.Reset();

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
	renderer->CreateBuffer(&lightBufferDesc, NULL, lightsBuffer.GetAddressOf());

	srvDesc.Format = DXGI_FORMAT_UNKNOWN; // Required for structured buffers
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = maxLights;

	renderer->CreateShaderResourceView(lightsBuffer.Get(), &srvDesc, lightBufferSRV.GetAddressOf());
}

void LightsDataModule::initModule() {
	setLightsBuffer();
}

void LightsDataModule::setModuleParamaters(ID3D11DeviceContext* deviceContext) {
	
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
		i++;
		});

	deviceContext->Unmap(lightsBuffer.Get(), 0);
}

void LightsDataModule::setResources(ID3D11DeviceContext* deviceContext, size_t startingRegister) {
	ID3D11ShaderResourceView* lightBufferSRVPtr = lightBufferSRV.Get();
	deviceContext->PSSetShaderResources(startingRegister, 1, &lightBufferSRVPtr);
}