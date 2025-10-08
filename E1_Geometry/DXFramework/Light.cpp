// Light class
// Holds data that represents a single light source
#include "Light.h"

//Constructor

Light::Light(lightTypes type) : 
	attenuation(0.05f, 0.01f, 0.001f, 100.f),
	diffuseColour(1.0f, 1.0f, 1.0f, 1.f),
	direction(XMVectorSet(0.0f, -1.f, 0.0f, 1.0f)),
	innerCone(XMConvertToRadians(20.f)),
	position(XMVectorSet(35.0f, 30.0f, 25.0f, 1.0f)),
	outerCone(XMConvertToRadians(40.f)),
	type(type) {}

// create view matrix, based on light position and lookat. Used for shadow mapping.
void Light::generateViewMatrix()
{
	XMFLOAT3 directionFloat = XMFLOAT3(XMVectorGetX(position), XMVectorGetY(position), XMVectorGetZ(position));

	// default up vector
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
	if (directionFloat.y == 1 || (directionFloat.x == 0 && directionFloat.z == 0))
	{
		up = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0);
	}
	else if (directionFloat.y == -1 || (directionFloat.x == 0 && directionFloat.z == 0))
	{
		up = XMVectorSet(0.0f, 0.0f, -1.0f, 1.0);
	}
	//XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
	XMVECTOR dir = XMVectorSet(directionFloat.x, directionFloat.y, directionFloat.z, 1.0f);
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

void Light::setType(lightTypes type) 
{
	this->type = type;
}

void Light::setAttenuation(float constant, float linear, float quadratic, float cutoffDistance)
{
	attenuation = XMFLOAT4(constant, linear, quadratic, cutoffDistance);
}

void Light::setAttenuation(const XMFLOAT4& attenuation)
{
	this->attenuation = attenuation;
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
	direction = XMVectorSet(x, y, z, 1.0f);
	direction = XMVector3Normalize(direction);
}

void Light::setDirection(const XMVECTOR& direction)
{
	this->direction = direction;
	this->direction = XMVector3Normalize(this->direction);
}

void Light::setDirection(const XMFLOAT3& direction)
{
	this->direction = XMVectorSet(direction.x, direction.y, direction.z, 1.0f);
	this->direction = XMVector3Normalize(this->direction);
}

void Light::setDirectionEuler(float x, float y, float z)
{

}
void Light::setDirectionEuler(const XMFLOAT3& rotation)
{
	const float& pitch = rotation.x;
	const float& yaw = rotation.y;
	const float& roll = rotation.z;


	XMFLOAT3 directionFloat;

	float x = sinf(rotation.y) * cosf(rotation.x);
	float y = -sinf(rotation.x);
	float z = cosf(rotation.y) * cosf(rotation.x);

	direction =  XMVector3Normalize(XMVectorSet(x, y, z, 1.0f));
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

void Light::setInnerCone(float innerCone)
{
	this->innerCone = innerCone;
}
void Light::setOuterCone(float outerCone)
{
	this->outerCone = outerCone;
}

void Light::setLookAt(float x, float y, float z)
{
	lookAt = XMVectorSet(x, y, z, 1.0f);
}

void Light::setLookAt(const XMFLOAT3& lookAt)
{
	this->lookAt = XMVectorSet(lookAt.x, lookAt.y, lookAt.z, 1.0f);
}



const XMFLOAT4& Light::getAttenuation() const
{
	return attenuation;
}

const XMFLOAT4& Light::getDiffuseColour() const
{
	return diffuseColour;
}


XMFLOAT3 Light::getDirection() const
{
	return XMFLOAT3(XMVectorGetX(direction), XMVectorGetY(direction), XMVectorGetZ(direction));
}

XMFLOAT3 Light::getDirectionEuler() const
{
	XMFLOAT3 angles;
	XMFLOAT3 directionFloat = getDirection();
	angles.y = atan2f(directionFloat.x, directionFloat.z); // Yaw
	angles.x = asinf(-directionFloat.y);				   // Pitch
	angles.z = 0.0f;									   // Roll - none for direction

	return angles;
}


XMFLOAT3 Light::getPosition() const
{
	return XMFLOAT3(XMVectorGetX(position), XMVectorGetY(position), XMVectorGetZ(position));
}

float Light::getInnerCone() const
{
	return innerCone;
}

float Light::getOuterCone() const
{
	return outerCone;
}

lightTypes Light::getType() const
{
	return type;
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

