#pragma once
#ifndef _MATERIAL_H_
#define _MATERIAL_H_

//#include <memory>
#include <directxmath.h>
#include <memory>
#include "TextureManager.h"

using namespace DirectX;

enum shaderTypes {
	light,
	manipulation
};

class BaseShader;

struct Material
{
	size_t liveUsers = 0;						///< numnber of current geometryInstances which use this material
	std::string MaterialName;					///< name of material
	XMFLOAT4 baseColour;						///< Colour value of texture;
	XMFLOAT4 specularColour;					///< Specular colour value
	float specularPower;						///< Specular power exponent
	size_t ShaderID;							///< Materials utilised materialID;
	std::wstring textureString;					///< String for materials texture
	std::shared_ptr<TextureResource> texture;	///< Pointer to materials texture
};

#endif