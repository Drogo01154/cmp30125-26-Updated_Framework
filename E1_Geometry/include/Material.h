#pragma once
#ifndef _MATERIAL_H_
#define _MATERIAL_H_

//#include <memory>
#include <directxmath.h>
#include <string>
#include <memory>

using namespace DirectX;

enum shaderTypes {
	lightShader
};

class BaseShader;

struct Material
{
	XMFLOAT4 specularColour;			///< Specular colour value
	float specularPower;				///< Specular power exponent
	std::wstring texture;				///< String for materials texture
	std::weak_ptr<BaseShader> shader;   ///< Pointer to shader used by material
};

#endif