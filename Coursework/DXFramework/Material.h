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
	float HeightMultiplier;						///< Scales heightmap displacement
	float UVScale;								///< Scales UVs for texture sampling
};
/*
	Material data struct contains objects;
		- Specular Colour and Power
		- Albedo colour
		- Diffuse Texture
		- Emissive Texture and strength
		- Height map data if material set to be a heightmap
*/
struct Material
{
	std::string MaterialName;					///< name of material
	XMFLOAT4 baseColour;						///< Colour value of texture;
	XMFLOAT4 specularColour;					///< Specular colour value
	float specularPower;						///< Specular power exponent
	float emissiveStrength;						///< Emission strength
	std::wstring diffuseTextureString;			///< Diffuse Colour access string
	std::wstring emissiveTextureString;			///< emission Texture access string
	TextureInstance diffuseTexture;				///< Materials diffuse texture
	TextureInstance emissionTexture;			///< Materials emission texture
	std::unique_ptr<HeightMapInfo> HeightMapData;	///< Ptr to height map data of plane.
};

#define MaterialInstance Instance<std::string, Material>

#endif