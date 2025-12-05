#pragma once
// Texture
// Loads and stores a texture ready for rendering.
// Handles mipmap generation on load.

#ifndef _TEXTUREMANAGER_H_
#define _TEXTUREMANAGER_H_

#include <d3d11.h>
#include "FileHandler.h"
#include "InstanceCache.h"
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
	std::wstring uid;
	ComPtr<ID3D11ShaderResourceView> texture;
	TextureResource(ComPtr<ID3D11ShaderResourceView> t, const std::wstring& uid) : texture(t) {}
};

#define TextureInstance Instance<std::wstring, TextureResource>

class TextureManager
{
public:
	TextureManager(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	~TextureManager();
	TextureInstance getTexture(const std::wstring& uid);
private:
	bool does_file_exist(const wchar_t *fileName);
	//void generateTexture(ID3D11Device* device);
	void addDefaultTexture();
	TextureInstance loadTexture(const std::wstring& uid, const std::wstring& filename);
	
	TextureInstance defaultTexture;	// Instance will always remain
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	InstanceCache<std::wstring, TextureResource> textureCache;
};

#endif