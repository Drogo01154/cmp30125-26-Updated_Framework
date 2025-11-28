// Light class
// Holds data that represents a single light source
#include "Light.h"

//Constructor

Light::Light() : Light(lightTypes::directional) {

}

Light::Light(lightTypes type) : 
	attenuation(0.05f, 0.01f, 0.001f, 100.f),
	diffuseColour(1.0f, 1.0f, 1.0f, 1.f),
	innerCone(XMConvertToRadians(20.f)),
	outerCone(XMConvertToRadians(40.f)),
	type(type) 
{
	m_transform.setPosition(35.f, 30.f, 25.f);

	// create a target point by adding a downward direction to the position
	XMVECTOR directionDown = XMVectorSet(0.f, -1.f, 0.f, 0.f);
	XMVECTOR target = XMVectorAdd(m_transform.getTranslationVector(), directionDown);
	m_transform.lookAt(target);
}

// create view matrix, based on light position and lookat. Used for shadow mapping.
void Light::generateViewMatrix()
{
	XMFLOAT3 directionFloat = getGlobalDirection();
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
	viewMatrix = XMMatrixLookAtLH(globalPosition, globalPosition + dir, up);
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

void Light::updateGlobals(bool updateTransform)
{
	if (updateTransform) { m_transform.computeGlobalMatrix(true); }
	XMMATRIX globalMatrix = m_transform.getGlobalMatrix();
	XMVECTOR outTranslation, outRotation, outScale;
	XMMatrixDecompose(&outScale, &outRotation, &outTranslation, globalMatrix);

	globalPosition = outTranslation;
	globalDirection = m_transform.getWorldForward();
}
XMFLOAT3 Light::getGlobalPosition() const 
{
	return XMFLOAT3(XMVectorGetX(globalPosition), XMVectorGetY(globalPosition), XMVectorGetZ(globalPosition));
}
XMFLOAT3 Light::getGlobalDirection() const
{
	return XMFLOAT3(XMVectorGetX(globalDirection), XMVectorGetY(globalDirection), XMVectorGetZ(globalDirection));
}

const XMVECTOR& Light::getGlobalPositionVector() const {
	return globalPosition;
}
const XMVECTOR& Light::getGlobalDirectionVector() const {
	return globalDirection;
}

std::span<const char* const> Light::GetLightTypeStrings() {
	return LightTypeStrings;
}

bool Light::imGuiRender(size_t transformIncrement) {
	bool lightUpdated = false;
	ImGui::Text(("Light Type: " + std::string(LightTypeStrings[type])).c_str());

	//Lights Diffuse Colour setting
	if (ImGui::TreeNode("Light Diffuse Colour")) {
		if (ImGui::ColorPicker4("Light Diffuse Colour: ", &diffuseColour.x)) { lightUpdated = true; }
		ImGui::TreePop();
	}

	if (m_transform.imGuiRender("Light Transform: ", transformIncrement, type != lightTypes::directional, type != lightTypes::point, false))
	{
		updateGlobals(true);
		lightUpdated = true;
	}

	if (type != lightTypes::directional)
	{
		if (ImGui::TreeNode("Attenuation")) {
			if (ImGui::DragFloat("Constant ", &attenuation.x, 0.1f, 1.0f, 1000.f, "%.2f")) { lightUpdated = true; }
			if (ImGui::DragFloat("Linear ", &attenuation.y, 0.01f, 0.01f, 0.15f, "%.2f")) { lightUpdated = true; }
			if (ImGui::DragFloat("Quadratic ", &attenuation.z, 0.0001f, 0.0001f, 0.2f, "%.4f")) { lightUpdated = true; }
			if (ImGui::DragFloat("Falloff ", &attenuation.w, 0.1f, 0.01f, 1000.f, "%.2f")) { lightUpdated = true; }
			ImGui::TreePop();
		}
	}

	if (type == lightTypes::spot) {
		if (ImGui::SliderAngle("Inner Cone: ", &innerCone, 0.f, 90.f)) { lightUpdated = true; }
		if (ImGui::SliderAngle("Outer Cone: ", &outerCone, 0.f, 90.f)) { lightUpdated = true; }
	}
	return lightUpdated;
}