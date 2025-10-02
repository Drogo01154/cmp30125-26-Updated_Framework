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
#include <directxmath.h>

using namespace DirectX;

class Light
{

public:
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
	void setAttenuation(float constant, float linear, float quadratic, float cutoffDistance);
	void setAttenuation(const XMFLOAT4& attenuation);
	void setAmbientColour(float red, float green, float blue, float alpha);		///< Set ambient colour RGBA
	void setAmbientColour(const XMFLOAT4& ambientColour);						//< Set ambient colour RGBA
	void setDiffuseColour(float red, float green, float blue, float alpha);		///< Set diffuse colour RGBA
	void setDiffuseColour(const XMFLOAT4& diffuseColour);						///< Set diffuse colour RGBA						
	void setDirection(float x, float y, float z);								///< Set light direction (for directional and spot lights)
	void setDirection(const XMFLOAT3& direction);								///< Set light direction (for directional and spot lights)
	void setPosition(float x, float y, float z);								///< Set light position (for point and spot lights)
	void setPosition(const XMFLOAT3& position);									///< Set light Position (for point and spot lights)
	void setPosition(const XMVECTOR& position);									///< set light position (for point and spot lights)
	void setLookAt(float x, float y, float z);									///< Set light lookAt (near deprecation)
	void setLookAt(const XMFLOAT3& lookAt);										///< Set light lookAt (near deprecation)
	void setInnerCone(float innerCone);											///< Set light inner cone (area lit up most)
	void setOuterCone(float outerCone);											///< Set light outer cone (area past which not lit)

	// Getters
	const XMFLOAT4& getAttenuation() const;			///< Get attenuation, returns float4
	const XMFLOAT4& getAmbientColour() const;		///< Get ambient colour, returns float4
	const XMFLOAT4& getDiffuseColour() const;		///< Get diffuse colour, returns float4
	const XMFLOAT3& getDirection() const;			///< Get light direction, returns float3
	XMFLOAT3 getPosition() const;					///< Get light position, returns XMVECTOR
	const XMMATRIX& getViewMatrix() const;			///< Get light view matrix for shadow mapping, returns XMMATRIX
	const XMMATRIX& getProjectionMatrix() const;	///< Get light projection matrix for shadow mapping, returns XMMATRIX
	const XMMATRIX& getOrthoMatrix() const;			///< Get light orthographic matrix for shadow mapping, returns XMMATRIX
	float getInnerCone() const;
	float getOuterCone() const;


protected:
	XMFLOAT4 attenuation; // Constant, Linear, Quadratic, Cutoff Distance
	XMFLOAT4 ambientColour;
	XMFLOAT4 diffuseColour;
	XMFLOAT3 direction;;
	XMVECTOR position;
	XMMATRIX viewMatrix;
	XMMATRIX projectionMatrix;
	XMMATRIX orthoMatrix;
	XMVECTOR lookAt;
	float innercone;
	float outercone;
};
#endif