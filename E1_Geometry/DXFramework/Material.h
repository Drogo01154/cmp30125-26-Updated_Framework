#pragma once
#include <memory>
#include <directxmath.h>

enum shaderTypes {
	lightShader
};

class BaseShader;

class Material
{
public:
	//Update later when add shader manager??
	Material(std::weak_ptr<BaseShader> shader, XMFLOAT4 specularColour = XMFLOAT4(1.f, 1.f, 1.f, 1.f), float specularPower = 32.0f, const std::wstring& texture = L"");


	//Getters 
	const XMFLOAT4& getSpecularColour() const;		///< Get specular colour, returns float4
	float getSpecularPower();						///< Get specular power, returns float
	const std::wstring& getTexture() const;			///< Get texture name, returns string
	std::weak_ptr<BaseShader> getShader();			///< Get materials shader from shader manager

	//Setters
	void setSpecularColour(float red, float green, float blue, float alpha);	///< set specular colour RGBA
	void setSpecularColour(const XMFLOAT4& specularColour);						///< set specular colour RGBA
	void setSpecularPower(float power);											///< Set specular power
	void setShader(std::weak_ptr<BaseShader> shader);							///< Set materials shader
	void setTexture(const std::wstring& texture);								///< Set materials texture

private:
	XMFLOAT4 specularColour;
	float specularPower;
	std::wstring texture;
	std::weak_ptr<BaseShader> shader;
};
