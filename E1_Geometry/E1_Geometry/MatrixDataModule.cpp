#include "MatrixDataModule.h"

MatrixDataModule::MatrixDataModule(ID3D11Device* device, HWND hwnd, InstanceManager* instanceManager) : BaseShaderModule(device, hwnd), instanceManager(instanceManager) {
	matrixBuffer = nullptr;
	initModule();
}

MatrixDataModule::~MatrixDataModule() {

}

void MatrixDataModule::initModule() {
	D3D11_BUFFER_DESC matrixBufferDesc;

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	renderer->CreateBuffer(&matrixBufferDesc, NULL, matrixBuffer.GetAddressOf());
}

void MatrixDataModule::setModuleParamaters(
	ID3D11DeviceContext* deviceContext,
	GeometryData* mesh, const XMMATRIX& projectionMatrix, bool orthCameraMat) {

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	

	HRESULT result;

	MatrixBufferType* dataPtr;

	XMMATRIX tworld, tview, tproj;

	// Transpose the matrices to prepare them for the shader.
	tworld = XMMatrixTranspose(mesh->m_transform.getGlobalMatrix());




	XMVECTOR scale;
	XMVECTOR rotationQuat;
	XMVECTOR translation;

	XMMatrixDecompose(&scale, &rotationQuat, &translation, tworld);

	// Print components
	XMFLOAT3 s, t;
	XMFLOAT4 r;
	XMStoreFloat3(&s, scale);
	XMStoreFloat3(&t, translation);
	XMStoreFloat4(&r, rotationQuat);



	if (orthCameraMat) {
		tview = XMMatrixTranspose(instanceManager->getActiveCamera()->camera->getOrthoViewMatrix());
	}
	else {
		tview = XMMatrixTranspose(instanceManager->getActiveCamera()->camera->getViewMatrix());
	}
	
	tproj = XMMatrixTranspose(projectionMatrix);
	result = deviceContext->Map(matrixBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	deviceContext->Unmap(matrixBuffer.Get(), 0);

}

void MatrixDataModule::setResources(D3D11_SHADER_VERSION_TYPE shaderType, ID3D11DeviceContext* deviceContext, size_t startingRegister) {

	ID3D11Buffer* matrixBufferPtr = matrixBuffer.Get();

	switch (shaderType) {
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_COMPUTE_SHADER:
		deviceContext->CSSetConstantBuffers(startingRegister, 1, &matrixBufferPtr);
		break;
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_DOMAIN_SHADER:
		deviceContext->DSSetConstantBuffers(startingRegister, 1, &matrixBufferPtr);
		break;
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_GEOMETRY_SHADER:
		deviceContext->GSSetConstantBuffers(startingRegister, 1, &matrixBufferPtr);
		break;
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_HULL_SHADER:
		deviceContext->HSSetConstantBuffers(startingRegister, 1, &matrixBufferPtr);
		break;
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_PIXEL_SHADER:
		deviceContext->PSSetConstantBuffers(startingRegister, 1, &matrixBufferPtr);
		break;
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_VERTEX_SHADER:
		deviceContext->VSSetConstantBuffers(startingRegister, 1, &matrixBufferPtr);
		break;
	default:
		throw std::runtime_error("Error: Shader type does not exist!");
	}
}