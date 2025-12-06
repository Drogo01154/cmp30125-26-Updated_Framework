#pragma once
#include "DXF.h"

using namespace std;
using namespace DirectX;

template<typename bufferType>
class ConstantDataModule : public BaseShaderModule {
private:

public:
	inline ConstantDataModule(ID3D11Device* device, HWND hwnd) : BaseShaderModule(device, hwnd) {

		static_assert(std::is_trivially_copyable_v<bufferType>, "Buffer type must be trivially copyable!");
		D3D11_BUFFER_DESC BufferDesc;

		// Setup the description of the dynamic camera constant buffer that is used in the vertex shader
		BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		BufferDesc.ByteWidth = sizeof(bufferType);
		BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		BufferDesc.MiscFlags = 0;
		BufferDesc.StructureByteStride = 0;
		HRESULT result = renderer->CreateBuffer(&BufferDesc, NULL, constantBuffer.GetAddressOf());
		if (FAILED(result)) { assert(false); }
	}

	inline void setModuleParamaters(ID3D11DeviceContext* deviceContext, const bufferType* buffer) {
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		deviceContext->Map(constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		memcpy(mappedResource.pData, buffer, sizeof(bufferType));
		deviceContext->Unmap(constantBuffer.Get(), 0);
	}

	inline void setResources(D3D11_SHADER_VERSION_TYPE shaderType, ID3D11DeviceContext* deviceContext, size_t startingRegister) {
		ID3D11Buffer* ConstantBufferPtr = constantBuffer.Get();

		switch (shaderType) {
		case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_COMPUTE_SHADER:
			deviceContext->CSSetConstantBuffers(startingRegister, 1, &ConstantBufferPtr);
			break;
		case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_DOMAIN_SHADER:
			deviceContext->DSSetConstantBuffers(startingRegister, 1, &ConstantBufferPtr);
			break;
		case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_GEOMETRY_SHADER:
			deviceContext->GSSetConstantBuffers(startingRegister, 1, &ConstantBufferPtr);
			break;
		case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_HULL_SHADER:
			deviceContext->HSSetConstantBuffers(startingRegister, 1, &ConstantBufferPtr);
			break;
		case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_PIXEL_SHADER:
			deviceContext->PSSetConstantBuffers(startingRegister, 1, &ConstantBufferPtr);
			break;
		case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_VERTEX_SHADER:
			deviceContext->VSSetConstantBuffers(startingRegister, 1, &ConstantBufferPtr);
			break;
		default:
			throw std::runtime_error("Error: Shader type does not exist!");
		}
	}
private:
	ComPtr<ID3D11Buffer> constantBuffer;
};
