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

std::shared_ptr<TextureResource> TextureManager::loadTexture(const std::wstring& uid, const std::wstring& filename)
{
	ComPtr<ID3D11ShaderResourceView> texture;
	HRESULT result;

	// check if file exists
	if (filename.empty())
	{
		//filename = L"../res/DefaultDiffuse.png";
		MessageBox(NULL, L"Texture filename does not exist", L"ERROR", MB_OK);
		return nullptr;
	}
	// if not set default texture
	if (!does_file_exist(filename.c_str()))
	{
		// change default texture
		//filename = L"../res/DefaultDiffuse.png";
		MessageBox(NULL, L"Texture filename does not exist", L"ERROR", MB_OK);
		return nullptr;
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
		return nullptr;
	}
	else
	{
		auto pair = textureMap.insert(std::make_pair(uid, std::make_shared<TextureResource>(texture)));
		textureLru.EmplaceReplace(uid, pair.first->second);
		return pair.first->second;
	}
}

// Release resource.
TextureManager::~TextureManager() {}

// Return texture as a shader resource.
std::shared_ptr<TextureResource> TextureManager::getTexture(const std::wstring& uid)
{
	//Attempt retrieval from LRU cache
	std::shared_ptr<TextureResource> retrievedTexture = textureLru.get(uid);
	//If not in LRU cache
	if (retrievedTexture == nullptr) {
		//Attept retrieval from texture map
		if (textureMap.find(uid) != textureMap.end())
		{
			// texture exists
			retrievedTexture = textureMap.at(uid);
		} else {
			//Attept to find texture in file handler map
			std::wstring* filePath = FileHandler::get().locateImage(uid);
			//Load texture if in file
			if (filePath != nullptr) {
				retrievedTexture = loadTexture(uid, *filePath);
				if (retrievedTexture == nullptr) {
					throw std::runtime_error("Texture:" + Converters::convert_from_wstring(uid) + " Could not be loaded!");
				}
			}
			else {
				throw std::runtime_error("Texture:" + Converters::convert_from_wstring(uid) + " does not exist!");
			}
		}
		textureLru.EmplaceReplace(uid, retrievedTexture); // Update LRU cache
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

void TextureManager::checkRemove(const std::wstring& uid) {
	if (uid == L"default") {
		throw std::runtime_error("Error: Cannot delete default texture!");
	}

	// Try to get from LRU cache first
	std::shared_ptr<TextureResource> TR = textureLru.get(uid);
	//Set internal refs to one for above local reference
	int internalRefs = 1;
	//If in LRU cache also in map so add 2 to references
	if(TR != nullptr) { internalRefs += 2; }
	else {
		
		auto it = textureMap.find(uid);
		//If in map increase internal refs and set TR
		if (it != textureMap.end()) 
		{ 
			TR = it->second;
			++internalRefs; 
		}
		else { return; }
	}
	//If use count only in manager class remove
	if (TR.use_count() == internalRefs) {
		// No external references, safe to remove
		textureLru.Remove(uid);
		textureMap.erase(uid);
	}
}

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

		hr = device->CreateShaderResourceView(pTexture.Get(), &SRVDesc, texture.GetAddressOf());
		textureMap.insert(std::make_pair(const_cast < wchar_t*>(L"default"), std::make_shared<TextureResource>(texture)));
	}
	
}