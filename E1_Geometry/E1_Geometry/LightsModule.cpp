#include "LightsModule.h"

LightsModule::LightsModule() {
	lightBufferSRV = nullptr;
	lightsBuffer = nullptr;
	sceneDataBuffer = nullptr;
	maxLights = 10;
}


LightsModule::~LightsModule() {

}

void LightsModule::setLightsBuffer() {
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
	renderer->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);
}