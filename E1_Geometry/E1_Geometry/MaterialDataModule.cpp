#include "MaterialDataModule.h"

MaterialDataModule::MaterialDataModule(ID3D11Device* device, HWND hwnd) :
	BaseShaderModule(device, hwnd) {
	D3D11_BUFFER_DESC MaterialDataBufferDesc;

	// Setup the description of the dynamic scebe data constant buffer that can be used in different states
	MaterialDataBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	MaterialDataBufferDesc.ByteWidth = sizeof(MaterialBufferType);
	MaterialDataBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	MaterialDataBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	MaterialDataBufferDesc.MiscFlags = 0;
	MaterialDataBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&MaterialDataBufferDesc, NULL, MaterialDataBuffer.GetAddressOf());
}
void MaterialDataModule::setModuleParamaters(ID3D11DeviceContext* deviceContext, Material* material) {
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MaterialBufferType* materialPtr;


	#define HASDIFFUSE   0x1  // 0001
	#define HASNORMAL    0x2  // 0010
	#define HASEMISSIVE  0x4  // 0100

	int flags = 0;  //No flags

	if (material->diffuseTexture.IsValid())
		flags |= HASDIFFUSE;   // set diffuse bit

	if (material->emissionTexture.IsValid())
		flags |= HASEMISSIVE;  // set emissive bit

	if (material->normalTexture.IsValid())
		flags |= HASNORMAL;    // set normal bit

	if (flags & HASEMISSIVE) {
		int test = 1;
	}


	//Send material data to buffer
	deviceContext->Map(MaterialDataBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	materialPtr = (MaterialBufferType*)mappedResource.pData;
	materialPtr->baseColour = material->baseColour;
	materialPtr->specularColour = material->specularColour;
	materialPtr->specularPower = material->specularPower;
	materialPtr->emissiveStrength = material->emissiveStrength;
	materialPtr->flags = flags;

	deviceContext->Unmap(MaterialDataBuffer.Get(), 0);
}

struct MaterialBufferType
{
	XMFLOAT4 baseColour;
	XMFLOAT4 specularColour;
	float specularPower;
	float emissiveStrength;
	int flags;
	XMFLOAT3 padding1;
};

void MaterialDataModule::setResources(D3D11_SHADER_VERSION_TYPE shaderType, ID3D11DeviceContext* deviceContext, size_t startingRegister) {

	ID3D11Buffer* buffers[] = { MaterialDataBuffer.Get() };
	switch (shaderType) {
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_COMPUTE_SHADER:
		deviceContext->CSSetConstantBuffers(startingRegister, 1, buffers);
		break;
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_DOMAIN_SHADER:
		deviceContext->DSSetConstantBuffers(startingRegister, 1, buffers);
		break;
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_GEOMETRY_SHADER:
		deviceContext->GSSetConstantBuffers(startingRegister, 1, buffers);
		break;
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_HULL_SHADER:
		deviceContext->HSSetConstantBuffers(startingRegister, 1, buffers);
		break;
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_PIXEL_SHADER:
		deviceContext->PSSetConstantBuffers(startingRegister, 1, buffers);
		break;
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_VERTEX_SHADER:
		deviceContext->VSSetConstantBuffers(startingRegister, 1, buffers);
		break;
	default:
		throw std::runtime_error("Error: Shader type does not exist!");
	}
}