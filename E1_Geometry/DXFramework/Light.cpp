// Light class
// Holds data that represents a single light source
#include "Light.h"

// create view matrix, based on light position and lookat. Used for shadow mapping.
void Light::generateViewMatrix()
{
	// default up vector
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
	if (direction.y == 1 || (direction.x == 0 && direction.z == 0))
	{
		up = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0);
	}
	else if (direction.y == -1 || (direction.x == 0 && direction.z == 0))
	{
		up = XMVectorSet(0.0f, 0.0f, -1.0f, 1.0);
	}
	//XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
	XMVECTOR dir = XMVectorSet(direction.x, direction.y, direction.z, 1.0f);
	XMVECTOR right = XMVector3Cross(dir, up);
	up = XMVector3Cross(right, dir);
	// Create the view matrix from the three vectors.
	viewMatrix = XMMatrixLookAtLH(position, position + dir, up);
}

// Create a projection matrix for the (point) light source. Used in shadow mapping.
void Light::generateProjectionMatrix(float screenNear, float screenFar)
{
	float fieldOfView, screenAspect;

	// Setup field of view and screen aspect for a square light source.
	fieldOfView = (float)XM_PI / 2.0f;
	screenAspect = 1.0f;

	// Create the projection matrix for the light.
	projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenFar);
}

// Create orthomatrix for (directional) light source. Used in shadow mapping.
void Light::generateOrthoMatrix(float screenWidth, float screenHeight, float near, float far)
{
	orthoMatrix = XMMatrixOrthographicLH(screenWidth, screenHeight, near, far);
}

void Light::setAttenuation(float constant, float linear, float quadratic, float cutoffDistance)
{
	attenuation = XMFLOAT4(constant, linear, quadratic, cutoffDistance);
}

void Light::setAttenuation(const XMFLOAT4& attenuation)
{
	this->attenuation = attenuation;
}

void Light::setAmbientColour(float red, float green, float blue, float alpha)
{
	ambientColour = XMFLOAT4(red, green, blue, alpha);
}

void Light::setAmbientColour(const XMFLOAT4& ambientColour)
{
	this->ambientColour = ambientColour;
}

void Light::setDiffuseColour(float red, float green, float blue, float alpha)
{
	diffuseColour = XMFLOAT4(red, green, blue, alpha);
}

void Light::setDiffuseColour(const XMFLOAT4& diffuseColour)
{
	this->diffuseColour = diffuseColour;
}

void Light::setDirection(float x, float y, float z)
{
	direction = XMFLOAT3(x, y, z);
}

void Light::setDirection(const XMFLOAT3& direction)
{
	this->direction = direction;
}

void Light::setPosition(float x, float y, float z)
{
	position = XMVectorSet(x, y, z, 1.0f);
}

void Light::setPosition(const XMFLOAT3& position)
{
	this->position = XMVectorSet(position.x, position.y, position.z, 1.0f);
}

void Light::setPosition(const XMVECTOR& position)
{
	this->position = position;
}

const XMFLOAT4& Light::getAttenuation() const
{
	return attenuation;
}

const XMFLOAT4& Light::getAmbientColour() const
{
	return ambientColour;
}

const XMFLOAT4& Light::getDiffuseColour() const
{
	return diffuseColour;
}


const XMFLOAT3& Light::getDirection() const
{
	return direction;
}


XMFLOAT3 Light::getPosition() const
{
	return XMFLOAT3(XMVectorGetX(position), XMVectorGetY(position), XMVectorGetZ(position));
}

void Light::setLookAt(float x, float y, float z)
{
	lookAt = XMVectorSet(x, y, z, 1.0f);
}

void Light::setLookAt(const XMFLOAT3& lookAt)
{
	this->lookAt = XMVectorSet(lookAt.x, lookAt.y, lookAt.z, 1.0f);
}

void Light::setInnerCone(float innerCone)
{
	this->innercone = innercone;
}
void Light::setOuterCone(float outerCone)
{
	this->outercone = outercone;
}

const XMMATRIX& Light::getViewMatrix() const
{
	return viewMatrix;
}

const XMMATRIX& Light::getProjectionMatrix() const
{
	return projectionMatrix;
}

const XMMATRIX& Light::getOrthoMatrix() const
{
	return orthoMatrix;
}

float Light::getInnerCone() const
{
	return innercone;
}

float Light::getOuterCone() const
{
	return outercone;
}