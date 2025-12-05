#include "HeightMapDataModule.h"

HeightMapDataModule::HeightMapDataModule(ID3D11Device* device, HWND hwnd) : BaseShaderModule(device, hwnd) {
	D3D11_BUFFER_DESC mapDataBufferDesc;

	// Setup the description of the map data constant buffer

	mapDataBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	mapDataBufferDesc.ByteWidth = sizeof(MapBufferType);
	mapDataBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	mapDataBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	mapDataBufferDesc.MiscFlags = 0;
	mapDataBufferDesc.StructureByteStride = 0;
	HRESULT timeResult = renderer->CreateBuffer(&mapDataBufferDesc, NULL, heightMapBuffer.GetAddressOf());

	if (FAILED(timeResult)) { assert(false); }
}

void HeightMapDataModule::setModuleParamaters(ID3D11DeviceContext* deviceContext, Material* material, GeometryData* plane) {


	if (plane->mesh->type != MeshType::PLANE) {
		throw std::runtime_error("Error: Geometry must be a plane!");
	}
	if (material->HeightMapData == nullptr) {
		throw std::runtime_error("Error: no height map material Data");
	}
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	ID3D11Resource* resource = nullptr;
	material->HeightMapData->HeightTexture->texture->GetResource(&resource);

	XMFLOAT2 pixelOffset;

	ID3D11Texture2D* texture2D = nullptr;
	HRESULT result = resource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&texture2D);
	if (SUCCEEDED(result) && texture2D)
	{
		D3D11_TEXTURE2D_DESC desc;
		texture2D->GetDesc(&desc);
		pixelOffset = { 1.f / desc.Width, 1.f / desc.Height };
	}
	else {
		throw std::runtime_error("Error, could not get Width/Height of texture!");
	}

	MapBufferType* heightMapPtr;
	deviceContext->Map(heightMapBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	heightMapPtr = (MapBufferType*)mappedResource.pData;
	heightMapPtr->heightMultiplier = material->HeightMapData->HeightMultiplier;
	int resolution = std::get<int>(plane->mesh->params["resolution"]);
	heightMapPtr->resolution = resolution;
	heightMapPtr->pixelOffset = pixelOffset;
	deviceContext->Unmap(heightMapBuffer.Get(), 0);

}


void HeightMapDataModule::setResources(D3D11_SHADER_VERSION_TYPE shaderType, ID3D11DeviceContext* deviceContext, size_t startingRegister) {
	ID3D11Buffer* heightBufferPtr = heightMapBuffer.Get();

	switch (shaderType) {
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_COMPUTE_SHADER:
		deviceContext->CSSetConstantBuffers(startingRegister, 1, &heightBufferPtr);
		break;
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_DOMAIN_SHADER:
		deviceContext->DSSetConstantBuffers(startingRegister, 1, &heightBufferPtr);
		break;
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_GEOMETRY_SHADER:
		deviceContext->GSSetConstantBuffers(startingRegister, 1, &heightBufferPtr);
		break;
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_HULL_SHADER:
		deviceContext->HSSetConstantBuffers(startingRegister, 1, &heightBufferPtr);
		break;
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_PIXEL_SHADER:
		deviceContext->PSSetConstantBuffers(startingRegister, 1, &heightBufferPtr);
		break;
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_VERTEX_SHADER:
		deviceContext->VSSetConstantBuffers(startingRegister, 1, &heightBufferPtr);
		break;
	default:
		throw std::runtime_error("Error: Shader type does not exist!");
	}
}

/*
	


*/