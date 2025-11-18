/**
* \class Light source
*
* \brief Represents a single light source
*
* Stores ambient, diffuse, specular colour, specular power, attenuation, . Also stores direction and position
* Additionally, generates view, projectiong and orthographics matrices for use in shadow mapping.
*
* \Original author Paul Robertson
* \Updated James Dobbie
*/


#ifndef _LIGHT_H_
#define _LIGHT_H_

// https://www.braynzarsoft.net/viewtutorial/q16390-21-spotlights
#include "Transform.h"
#include <span>

using namespace DirectX;

enum lightTypes {
	directional,
	point,
	spot
};
		
class Light
{

public:
	Light();
	Light(lightTypes type);
	
	void* operator new(size_t i)
	{
		return _mm_malloc(i, 16);
	}

	void operator delete(void* p)
	{
		_mm_free(p);
	}

	void generateViewMatrix();																	///< Generates and upto date view matrix, based on current rotation
	void generateProjectionMatrix(float screenNear, float screenFar);							///< Generate project matrix based on current rotation and provided near & far plane
	void generateOrthoMatrix(float screenWidth, float screenHeight, float near, float far);		///< Generates orthographic matrix based on supplied screen dimensions and near & far plane.

	// Setters
	void setType(lightTypes type);
	void setAttenuation(float constant, float linear, float quadratic, float cutoffDistance);
	void setAttenuation(const XMFLOAT4& attenuation);
	void setDiffuseColour(float red, float green, float blue, float alpha);		///< Set diffuse colour RGBA
	void setDiffuseColour(const XMFLOAT4& diffuseColour);						///< Set diffuse colour RGBA						
	void setLookAt(float x, float y, float z);									///< Set light lookAt (near deprecation)
	void setLookAt(const XMFLOAT3& lookAt);										///< Set light lookAt (near deprecation)
	void setInnerCone(float innerCone);											///< Set light inner cone (area lit up most)
	void setOuterCone(float outerCone);											///< Set light outer cone (area past which not lit)

	// Getters
	const XMFLOAT4& getAttenuation() const;			///< Get attenuation, returns float4
	const XMFLOAT4& getDiffuseColour() const;		///< Get diffuse colour, returns float4
	const XMMATRIX& getViewMatrix() const;			///< Get light view matrix for shadow mapping, returns XMMATRIX
	const XMMATRIX& getProjectionMatrix() const;	///< Get light projection matrix for shadow mapping, returns XMMATRIX
	const XMMATRIX& getOrthoMatrix() const;			///< Get light orthographic matrix for shadow mapping, returns XMMATRIX

	void updateGlobals(bool updateTransform = true);	///> Set global values from transform and parent matrix
	XMFLOAT3 getGlobalPosition() const;				///> Get lights global translation
	XMFLOAT3 getGlobalDirection() const;			///> Get lights global rotation

	float getInnerCone() const;						///< Get light Inner cone angle, returns float
	float getOuterCone() const;						///< Get light Outer cone angle, returns float
	lightTypes getType() const;						///< Get light type, returns enum

	void imGuiRender(size_t transformIncrement);

	static std::span<const char* const> GetLightTypeStrings();

	Transform m_transform;
protected:
	XMFLOAT4 attenuation; // Constant, Linear, Quadratic, Cutoff Distance
	XMFLOAT4 diffuseColour;
	float innerCone;
	XMMATRIX viewMatrix;
	XMMATRIX projectionMatrix;
	XMMATRIX orthoMatrix;
	XMVECTOR lookAt;
	XMVECTOR globalPosition;
	XMVECTOR globalDirection;
	
	float outerCone;
	lightTypes type;
	float padding[2]; // pad to 16-byte boundary

	inline static const char* LightTypeStrings[3] =
	{
		"directional",
		"point",
		"spot"
	};
};

namespace nlohmann {
	template<>
	struct adl_serializer<Light> {
		static void to_json(json& j, const Light& l) {
			j["type"] = l.getType();
			j["attenuation"] = l.getAttenuation();
			j["diffuseColour"] = l.getDiffuseColour();
			j["innerCone"] = l.getInnerCone();
			j["outerCone"] = l.getOuterCone();
			j["transform"] = l.m_transform;
		}

		static void from_json(const json& j, Light& l) {
			l.setType(j.at("type").get<lightTypes>());
			l.setAttenuation(j.at("attenuation").get<XMFLOAT4>());
			l.setDiffuseColour(j.at("diffuseColour").get<XMFLOAT4>());
			l.setInnerCone(j.at("innerCone").get<float>());
			l.setOuterCone(j.at("outerCone").get<float>());
			l.m_transform = j.at("transform").get<Transform>();
		}
	};
}


#endif