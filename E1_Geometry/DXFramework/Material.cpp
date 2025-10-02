#include "Material.h"

Material::Material(std::weak_ptr<BaseShader> shader, XMFLOAT4 specularColour, float specularPower, const std::wstring& texture) : shader(shader), specularColour(specularColour), specularPower(specularPower), texture(texture)
{

}

const XMFLOAT4& Material::getSpecularColour() const
{
	return specularColour;
}


float Material::getSpecularPower()
{
	return specularPower;
}

const std::wstring& Material::getTexture() const
{
	return texture;
}

std::weak_ptr<BaseShader> Material::getShader()
{
	return shader;
}

void Material::setSpecularColour(float red, float green, float blue, float alpha)
{
	specularColour = XMFLOAT4(red, green, blue, alpha);
}

void Material::setSpecularColour(const XMFLOAT4& specularColour)
{
	this->specularColour = specularColour;
}

void Material::setSpecularPower(float power)
{
	specularPower = power;
}

void Material::setShader(std::weak_ptr<BaseShader> shader) 
{
	this->shader = shader;
}

void Material::setTexture(const std::wstring& texture) 
{
	this->texture = texture;
}