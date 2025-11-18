#include "albedoLightShader.h"
AlbedoLightShader::AlbedoLightShader(ShaderManager* shaderManager, InstanceManager* instanceManager, ID3D11Device* device, HWND hwnd) :
	BaseShader(device, hwnd),
	shaderManager(shaderManager),
	instanceManager(instanceManager)

{
	size_t matrixModuleID = shaderManager->getShaderModuleID("MatrixDataModule");
	matrixDataModule = std::dynamic_pointer_cast<MatrixDataModule>(shaderManager->getShaderModule(matrixModuleID));
	shaderManager->addShaderModuleInstanceReference(matrixModuleID);
	initShader(L"light_vs.cso", L"albedoLight_ps.cso");
}


AlbedoLightShader::~AlbedoLightShader()
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


void AlbedoLightShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
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
}

void AlbedoLightShader::setShaderParamaters(
	ID3D11DeviceContext* deviceContext,
	const XMMATRIX& worldMatrix,
	const XMMATRIX& viewMatrix,
	const XMMATRIX& projectionMatrix,
	std::shared_ptr<Material> material,
	std::vector<std::shared_ptr<Light>>& lights,
	const XMFLOAT4& ambient,
	Camera* camera)
{
	matrixDataModule->
	matrixDataModule.setModuleParamaters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
	lightsDataModule.setModuleParamaters(deviceContext, material, lights, ambient);

	D3D11_MAPPED_SUBRESOURCE mappedResource;

	//Additional
	// Send camera data to vertex shader
	CameraBufferType* cameraPtr;
	deviceContext->Map(cameraBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	cameraPtr = (CameraBufferType*)mappedResource.pData;
	cameraPtr->cameraPosition = camera->getGlobalPosition();
	deviceContext->Unmap(cameraBuffer.Get(), 0);
	ID3D11Buffer* cameraBufferPtr = cameraBuffer.Get();
	deviceContext->VSSetConstantBuffers(1, 1, &cameraBufferPtr);
}