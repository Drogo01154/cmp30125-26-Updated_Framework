#pragma once
// Texture
// Loads and stores a texture ready for rendering.
// Handles mipmap generation on load.

#ifndef _TEXTUREMANAGER_H_
#define _TEXTUREMANAGER_H_

#include <d3d11.h>
#include "FileHandler.h"
#include "LRUCache.h"
//#include "../DirectXTK/Inc/DDSTextureLoader.h"
//#include "../DirectXTK/Inc/WICTextureLoader.h"
#include "DTK\include\DDSTextureLoader.h"
#include "DTK\include\WICTextureLoader.h"
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <wrl/client.h>

//#include "Texture.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;

struct TextureResource {
	ComPtr<ID3D11ShaderResourceView> texture;
	TextureResource(ComPtr<ID3D11ShaderResourceView> t) : texture(t) {}
};

class TextureManager
{
public:
	TextureManager(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	~TextureManager();
	std::shared_ptr<TextureResource> getTexture(const std::wstring& uid);
	void checkRemove(const std::wstring& uid); //Unloads texture if no more in scene
private:
	bool does_file_exist(const wchar_t *fileName);
	//void generateTexture(ID3D11Device* device);
	void addDefaultTexture();
	std::shared_ptr<TextureResource> loadTexture(const std::wstring& uid, const std::wstring& filename);

	
	
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	LRUCache<std::wstring, std::shared_ptr<TextureResource>> textureLru;
	std::map<std::wstring, std::shared_ptr<TextureResource>> textureMap;
};

#endif