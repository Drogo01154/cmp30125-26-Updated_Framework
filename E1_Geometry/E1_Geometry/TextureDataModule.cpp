#include "TextureDataModule.h"
TextureDataModule::TextureDataModule(ID3D11Device* device, HWND hwnd) : BaseShaderModule(device, hwnd) {
	texturePtr = nullptr;
}

void TextureDataModule::setModuleParamaters(ID3D11DeviceContext* deviceContext, TextureResource* textureResource) {
	texturePtr = textureResource->texture.Get();
}
void TextureDataModule::setModuleParamaters(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* texture) {
	texturePtr = texture;
}

void TextureDataModule::setResources(D3D11_SHADER_VERSION_TYPE shaderType, ID3D11DeviceContext* deviceContext, size_t startingRegister) {
	ID3D11ShaderResourceView* srvArray[1] = { texturePtr }; // texturePtr may be nullptr
	switch (shaderType) {
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_COMPUTE_SHADER:
		deviceContext->CSSetShaderResources(startingRegister, 1, srvArray);
		break;
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_DOMAIN_SHADER:
		deviceContext->DSSetShaderResources(startingRegister, 1, srvArray);
		break;
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_GEOMETRY_SHADER:
		deviceContext->GSSetShaderResources(startingRegister, 1, srvArray);
		break;
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_HULL_SHADER:
		deviceContext->HSSetShaderResources(startingRegister, 1, srvArray);
		break;
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_PIXEL_SHADER:
		deviceContext->PSSetShaderResources(startingRegister, 1, srvArray);
		break;
	case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_VERTEX_SHADER:
		deviceContext->VSSetShaderResources(startingRegister, 1, srvArray);
		break;
	default:
		throw std::runtime_error("Error: Shader type does not exist!");
	}
}