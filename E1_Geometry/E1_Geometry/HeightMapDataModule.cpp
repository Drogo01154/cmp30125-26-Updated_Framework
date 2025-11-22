#include "HeightMapDataModule.h"

HeightMapDataModule::HeightMapDataModule(ID3D11Device* device, HWND hwnd) :  {

}
HeightMapDataModule::~HeightMapDataModule() {

}

void HeightMapDataModule::initModule() {

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

void HeightMapDataModule::setModuleParamaters(ID3D11DeviceContext* deviceContext, TextureResource* Heightmap, Material* material, GeometryData* plane) {


	if (plane->mesh->type != MeshType::PLANE) {
		throw std::runtime_error("Error: Geometry must be a plane!");
	}
	if (material->)
	TextureResource* Heightmap 
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	ID3D11Resource* resource = nullptr;
	Heightmap->texture->GetResource(&resource);

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

	
}


void HeightMapDataModule::setResources(D3D11_SHADER_VERSION_TYPE shaderType, ID3D11DeviceContext* deviceContext, size_t startingRegister) {

}

/*
	


*/