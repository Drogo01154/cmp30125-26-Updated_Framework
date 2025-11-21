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

struct Material
{
	std::string MaterialName;					///< name of material
	XMFLOAT4 baseColour;						///< Colour value of texture;
	XMFLOAT4 specularColour;					///< Specular colour value
	float specularPower;						///< Specular power exponent
	std::string shaderName;
	ShaderInstance shader;						///< Materials Geometry Shader
	std::wstring textureString;
	TextureInstance texture;					///< Materials texture
};

#define MaterialInstance Instance<std::string, Material>

#endif