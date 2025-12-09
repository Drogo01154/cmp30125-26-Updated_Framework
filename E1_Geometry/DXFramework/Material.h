#pragma once
#ifndef _MATERIAL_H_
#define _MATERIAL_H_

//#include <memory>
#include <directxmath.h>
#include <memory>
#include "TextureManager.h"
#include "ShaderManager.h"

using namespace DirectX;

enum shaderTypes {
	light,
	manipulation
};

class BaseShader;

struct HeightMapInfo {
	HeightMapInfo() : HeightTextureString(L"height"), HeightMultiplier(10.f), UVScale(1) {}
	std::wstring HeightTextureString;
	TextureInstance HeightTexture;
	float HeightMultiplier;
	float UVScale;								///< Scales UVs for texture sampling
};

struct Material
{
	std::string MaterialName;					///< name of material
	XMFLOAT4 baseColour;						///< Colour value of texture;
	XMFLOAT4 specularColour;					///< Specular colour value
	float specularPower;						///< Specular power exponent
	float emissiveStrength;						///< Emission strength
	std::wstring diffuseTextureString;			///< Diffuse Colour access string
	std::wstring normalTextureString;			///< normal Texture access string
	std::wstring emissiveTextureString;			///< emission Texture access string
	TextureInstance diffuseTexture;				///< Materials diffuse texture
	TextureInstance normalTexture;				///< Materials normal texture
	TextureInstance emissionTexture;			///< Materials emission texture
	std::unique_ptr<HeightMapInfo> HeightMapData;	///< Ptr to height map data of plane.
};

#define MaterialInstance Instance<std::string, Material>

#endif