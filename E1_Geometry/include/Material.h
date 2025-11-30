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
	std::wstring HeightTextureString;
	TextureInstance HeightTexture;
	float HeightMultiplier;
};

struct Material
{
	std::string MaterialName;					///< name of material
	XMFLOAT4 baseColour;						///< Colour value of texture;
	XMFLOAT4 specularColour;					///< Specular colour value
	float specularPower;						///< Specular power exponent
	std::wstring textureString;
	TextureInstance texture;					///< Materials texture
	std::unique_ptr<HeightMapInfo> HeightMapData;	///< Ptr to height map data of plane.
};

#define MaterialInstance Instance<std::string, Material>

#endif