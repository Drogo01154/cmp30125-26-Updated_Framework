#include "LightsDataModule.h"

LightsDataModule::LightsDataModule(ID3D11Device* device, HWND hwnd) : BaseShaderModule(device, hwnd) {
	lightBufferSRV = nullptr;
	lightsBuffer = nullptr;
	sceneDataBuffer = nullptr;
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
	D3D11_BUFFER_DESC sceneDataBufferDesc;

	// Setup the description of the dynamic world data constant buffer that is used in the pixel shader. 
	sceneDataBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	sceneDataBufferDesc.ByteWidth = sizeof(SceneBufferType);
	sceneDataBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	sceneDataBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	sceneDataBufferDesc.MiscFlags = 0;
	sceneDataBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&sceneDataBufferDesc, NULL, sceneDataBuffer.GetAddressOf());
	setLightsBuffer();
}

void LightsDataModule::setModuleParamaters(ID3D11DeviceContext* deviceContext, std::shared_ptr<Material> material, std::vector<std::shared_ptr<Light>>& lights, const XMFLOAT4& ambient) {
	
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	int numLights = lights.size();

	if (numLights > maxLights * expandThreshold) {
		maxLights *= 2;
		setLightsBuffer();
	}
	else if ((maxLights > 10) && numLights < maxLights * shrinkThreshold) {
		maxLights /= 2;
		setLightsBuffer();
	}

	// Send world data to pixel shader
	SceneBufferType* scenePtr;
	deviceContext->Map(sceneDataBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	scenePtr = (SceneBufferType*)mappedResource.pData;
	scenePtr->baseColour = material->baseColour;
	scenePtr->ambientLight = ambient;
	scenePtr->numberOfLights = numLights;
	scenePtr->specular = material->specularColour;
	scenePtr->specularPower = material->specularPower;

	deviceContext->Unmap(sceneDataBuffer.Get(), 0);
	deviceContext->PSSetConstantBuffers(0, 1, sceneDataBuffer.GetAddressOf());

	// Send light data to pixel shader
	LightBufferType* lightPtr;
	deviceContext->Map(lightsBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	lightPtr = (LightBufferType*)mappedResource.pData;

	for (int i = 0; i < maxLights; i++)
	{
		if (i < lights.size())
		{
			lightPtr[i].attenuation = lights[i]->getAttenuation();
			lightPtr[i].position = lights[i]->getPosition();
			lightPtr[i].direction = lights[i]->getDirectionNormalized();
			lightPtr[i].innerCone = cos(lights[i]->getInnerCone());
			lightPtr[i].outerCone = cos(lights[i]->getOuterCone());
			lightPtr[i].diffuse = lights[i]->getDiffuseColour();
			lightPtr[i].type = static_cast<int>(lights[i]->getType());
		}
		else {
			// Zero out the remaining lights
			lightPtr[i] = {};
		}


	}

	deviceContext->Unmap(lightsBuffer.Get(), 0);
	deviceContext->PSSetShaderResources(0, 1, lightBufferSRV.GetAddressOf());

}