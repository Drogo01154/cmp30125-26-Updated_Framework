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
	Light() {}
	Light(lightTypes type);
	
	void* operator new(size_t i)
	{
		return _mm_malloc(i, 16);
	}

	void operator delete(void* p)
	{
		_mm_free(p);
	}

	void generateViewMatrix();									///< Generates and upto date view matrix, based on current rotation
	void generateProjectionMatrix(float aspect);				///< Generate project matrix based on current rotation and provided near & far plane

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
	const XMMATRIX& getViewMatrix(int face = 0) const;			///< Get light view matrix for shadow mapping, returns XMMATRIX
	const XMMATRIX& getProjectionMatrix() const;	///< Get light projection matrix for shadow/cube mapping, returns XMMATRIX

	void updateGlobals(bool updateTransform = true);	///> Set global values from transform and parent matrix
	XMFLOAT3 getGlobalPosition() const;					///> Get lights global translation as float3
	XMFLOAT3 getGlobalDirection() const;				///> Get lights global direction as float3
	const XMVECTOR& getGlobalPositionVector() const;	///> Get lights global translation as XMVECRTOR
	const XMVECTOR& getGlobalDirectionVector() const;	///> Get light global direction as XMVECTOR;

	float getInnerCone() const;						///< Get light Inner cone angle, returns float
	float getOuterCone() const;						///< Get light Outer cone angle, returns float
	lightTypes getType() const;						///< Get light type, returns enum

	bool imGuiRender(size_t transformIncrement, bool& projectionChanged);

	static std::span<const char* const> GetLightTypeStrings();

	void setFar(float inFar);
	void setNear(float inNear);
	void setFOV(float fov);
	void setOrthoWidth(float orthoWidth);
	void setOrthoHeight(float orthoHeight);
	void setConstBias(float constBias);
	void setSlopeBias(float slopeBias);

	float getFar() const;
	float getNear() const;
	float getFOV() const;
	float getOrthoWidth() const;
	float getOrthoHeight() const;
	float getConstBias() const;
	float getSlopeBias() const;


	Transform m_transform;
protected:
	XMFLOAT4 attenuation; // Constant, Linear, Quadratic, Cutoff Distance
	XMFLOAT4 diffuseColour;
	float innerCone;
	std::vector<XMMATRIX> viewMatrixes;
	XMMATRIX projectionMatrix;
	XMVECTOR lookAt;
	XMVECTOR globalPosition;
	XMVECTOR globalDirection;
	
	float outerCone;
	lightTypes type;

	float lightNear;
	float lightFar;

	//Spot light only
	float FOV;

	float minConstBias;
	float maxConstBias;
	float constBias;

	float minSlopeBias;
	float maxSlopeBias;
	float slopeBias;

	//Direction Light only
	float orthoWidth;
	float orthoHeight;

	float padding[2]; // pad to 16-byte boundary

	inline static const char* LightTypeStrings[3] =
	{
		"directional",
		"point",
		"spot"
	};

	struct Directions {
		Directions() {
			XMFLOAT3 floatDirs[6] = {
			{ 1, 0, 0 }, {-1, 0, 0 },
			{ 0, 1, 0 }, { 0,-1, 0 },
			{ 0, 0, 1 }, { 0, 0,-1 } };

			XMFLOAT3 floatUps[6] = {
			{ 0,-1, 0 }, { 0,-1, 0 },
			{ 0, 0, 1 }, { 0, 0,-1 },
			{ 0,-1, 0 },{ 0,-1, 0 } };

			for (int face = 0; face < 6; ++face) {
				forwards[face] = XMLoadFloat3(&floatDirs[face]);
				ups[face] = XMLoadFloat3(&floatUps[face]);
			}

		}

		XMVECTOR ups[6];
		XMVECTOR forwards[6];
	};

	inline static Directions directions;
};

namespace nlohmann {
	template<>
	struct adl_serializer<Light> {
		static void to_json(json& j, const Light& l) {
			lightTypes type = l.getType();
			j["type"] = type;
			j["attenuation"] = l.getAttenuation();
			j["diffuseColour"] = l.getDiffuseColour();
			j["innerCone"] = l.getInnerCone();
			j["outerCone"] = l.getOuterCone();
			j["transform"] = l.m_transform;
			j["far"] = l.getFar();
			j["near"] = l.getNear();
			j["constBias"] = l.getConstBias();
			j["slopeBias"] = l.getSlopeBias();
			if (type == lightTypes::spot) {
				j["FOV"] = l.getFOV();
			}
			else if (type == lightTypes::directional) {
				j["orthoWidth"] = l.getOrthoWidth();
				j["orthoHeight"] = l.getOrthoHeight();
			}
		}

		static void from_json(const json& j, Light& l) {
			lightTypes type = j.at("type").get<lightTypes>();
			l.setType(type);
			l.setAttenuation(j.at("attenuation").get<XMFLOAT4>());
			l.setDiffuseColour(j.at("diffuseColour").get<XMFLOAT4>());
			l.setInnerCone(j.at("innerCone").get<float>());
			l.setOuterCone(j.at("outerCone").get<float>());
			l.m_transform = j.at("transform").get<Transform>();
			l.setFar(j.at("innerCone").get<float>());
			l.setNear(j.at("outerCone").get<float>());
			l.setConstBias(j.at("constBias").get<float>());
			l.setSlopeBias(j.at("slopeBias").get<float>());
			if (type == lightTypes::spot) {
				l.setFOV(j.at("FOV").get<float>());
			}
			else if (type == lightTypes::directional) {
				l.setOrthoWidth(j.at("orthoWidth").get<float>());
				l.setOrthoHeight(j.at("orthoHeight").get<float>());
			}
		}
	};
}


#endif