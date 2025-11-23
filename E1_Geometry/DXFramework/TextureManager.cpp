// texture
// Loads and stores a single texture.
// Handles .dds, .png and .jpg (probably).
#include "TextureManager.h"
#include "Converters.h"


//Attempt to load texture. If load fails use default texture.
//Based on extension, uses slightly different loading function for different image types .dds vs .png/.jpg.
TextureManager::TextureManager(ID3D11Device* ldevice, ID3D11DeviceContext* ldeviceContext)
{
	device = ldevice;
	deviceContext = ldeviceContext;

	addDefaultTexture();
}

TextureInstance TextureManager::loadTexture(const std::wstring& uid, const std::wstring& filename)
{
	ComPtr<ID3D11ShaderResourceView> texture;
	HRESULT result;

	// check if file exists
	if (filename.empty())
	{
		//filename = L"../res/DefaultDiffuse.png";
		MessageBox(NULL, L"Texture filename does not exist", L"ERROR", MB_OK);
		return TextureInstance();
	}
	// if not set default texture
	if (!does_file_exist(filename.c_str()))
	{
		// change default texture
		//filename = L"../res/DefaultDiffuse.png";
		MessageBox(NULL, L"Texture filename does not exist", L"ERROR", MB_OK);
		return TextureInstance();
	}

	// check file extension for correct loading function.
	std::wstring fn(filename);
	std::string::size_type idx;
	std::wstring extension;

	idx = fn.rfind('.');

	if (idx != std::string::npos)
	{
		extension = fn.substr(idx + 1);
	}
	else
	{
		// No extension found
	}

	// Load the texture in.
	if (extension == L"dds")
	{
		result = CreateDDSTextureFromFile(device, deviceContext, filename.c_str(), NULL, texture.GetAddressOf());
	}
	else
	{
		result = CreateWICTextureFromFile(device, deviceContext, filename.c_str(), NULL, texture.GetAddressOf(), 0);
	}
	
	if (FAILED(result))
	{
		MessageBox(NULL, L"Texture loading error", L"ERROR", MB_OK);
		return TextureInstance();
	}
	else
	{
		return textureCache.emplaceID(uid, TextureResource(texture, uid));
	}
}

// Release resource.
TextureManager::~TextureManager() {}

std::wstring TextureManager::getTextureNameFromIndex(size_t index) {
	return Converters::convert_to_wstring(textureCache.getCachedName(index));
}

// Return texture as a shader resource.
TextureInstance TextureManager::getTexture(const std::wstring& uid)
{
	//Attempt retrieval from LRU cache
	TextureInstance retrievedTexture = textureCache.tryGetInstance(uid);
	if (!retrievedTexture.IsValid()) {
		std::wstring* filePath = FileHandler::get().locateImage(uid);
		retrievedTexture = loadTexture(uid, *filePath);
		if (!retrievedTexture.IsValid()) {
			throw std::runtime_error("Texture:" + Converters::convert_from_wstring(uid) + " Could not be loaded!");
		}
	}
	return retrievedTexture;
}

bool TextureManager::does_file_exist(const wchar_t *fname)
{
	std::ifstream infile(fname);
	return infile.good();
}

/*
void TextureManager::generateTexture(ID3D11Device* device)
{
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = 256;
	desc.Height = 256;
	desc.MipLevels = desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;

	device->CreateTexture2D(&desc, NULL, pTexture.GetAddressOf());
}
*/

void TextureManager::addDefaultTexture()
{
	ComPtr<ID3D11ShaderResourceView> texture;
	ComPtr<ID3D11Texture2D> pTexture;
	static const uint32_t s_pixel = 0xffffffff;

	D3D11_SUBRESOURCE_DATA initData = { &s_pixel, sizeof(uint32_t), 0 };

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = desc.Height = desc.MipLevels = desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	HRESULT hr = device->CreateTexture2D(&desc, &initData, pTexture.GetAddressOf());

	if (SUCCEEDED(hr))
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = 1;
		std::wstring defaultUid = L"default";
		defaultTexture = textureCache.emplaceID(defaultUid, TextureResource(texture, defaultUid));
		hr = device->CreateShaderResourceView(pTexture.Get(), &SRVDesc, texture.GetAddressOf());
	}	
}