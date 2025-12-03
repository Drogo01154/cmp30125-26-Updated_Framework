// Light class
// Holds data that represents a single light source
#include "Light.h"

//Constructor


Light::Light(lightTypes type) :
	attenuation(0.05f, 0.01f, 0.001f, 100.f),
	diffuseColour(1.0f, 1.0f, 1.0f, 1.f),
	innerCone(XMConvertToRadians(8.f)),
	outerCone(XMConvertToRadians(20.f)),
	type(type) 
{
	m_transform.setPosition(10.f, 5, 10.f);

	// create a target point by adding a downward direction to the position
	XMVECTOR directionDown = XMVectorSet(0.f, -1.f, 0.f, 0.f);
	XMVECTOR target = XMVectorAdd(m_transform.getTranslationVector(), directionDown);
	m_transform.lookAt(target);

	lightNear = 0.1f;
	lightFar = 25.0f;

	switch (type) {
	case lightTypes::point:
		viewMatrixes.resize(6);

		minConstBias = 0.003f;   // slider minimum
		maxConstBias = 0.005f;   // slider maximum
		minSlopeBias = 0.5f;    // slider minimum
		maxSlopeBias = 1.0f;    // slider maximum
		break;

	case lightTypes::directional:
		viewMatrixes.resize(1);
		orthoWidth = 50.f;
		orthoHeight = 50.f;

		minConstBias = 0.0001f;   // tiny, just to avoid acne
		maxConstBias = 0.002f;    // small enough to avoid floating shadows

		minSlopeBias = 0.05f;     // shallow slopes
		maxSlopeBias = 0.5f;      // steep slopes
		break;
	case lightTypes::spot:
		viewMatrixes.resize(1);
		FOV = XMConvertToRadians(45.0f); // 45° cone

		minConstBias = 0.0001;
		maxConstBias = 0.001;

		minSlopeBias = 0.05;
		maxSlopeBias = 1.0f;
		break;
	default:
		throw std::runtime_error("Error: Light type does not exist!");
	}

	constBias = minConstBias;
	slopeBias = minSlopeBias;
}

// create view matrix, based on light position and lookat. Used for shadow mapping.
void Light::generateViewMatrix()
{

	if (type == lightTypes::point) {
		for (int face = 0; face < 6; ++face)
		{
			viewMatrixes[face] = XMMatrixLookAtLH(
				globalPosition,                       // Camera position
				globalPosition + directions.forwards[face], // Look at direction
				directions.ups[face]                     // Up vector
			);
		}
	}
	else {
		XMVECTOR forward = cleanVector3(m_transform.getWorldForward());
		XMVECTOR up = cleanVector3(m_transform.getWorldUp());
		// Create the view matrix from the three vectors.
		viewMatrixes[0] = XMMatrixLookAtLH(globalPosition, globalPosition + forward, up);
	}
}

// Create a projection matrix for the light source. Used in shadow mapping.
void Light::generateProjectionMatrix(float aspect)
{
	switch (type) {
	case lightTypes::point:
	{
		float fieldOfView = (float)XM_PI / 2.0f;
		float screenAspect = 1.0f;
		// Create the projection matrix for the light.
		projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, lightNear, lightFar);
	}
		break;
	case lightTypes::directional:
		projectionMatrix = XMMatrixOrthographicLH(orthoWidth, orthoHeight, lightNear, lightFar);
		break;

	case lightTypes::spot:
		projectionMatrix = XMMatrixPerspectiveFovLH(FOV, aspect, lightNear, lightFar);
		break;
	}
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

const XMMATRIX& Light::getViewMatrix(int face) const
{
	return viewMatrixes[face];
}

const XMMATRIX& Light::getProjectionMatrix() const
{
	return projectionMatrix;
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

void Light::setFar(float inFar) { lightFar = inFar; }
void Light::setNear(float inNear) { lightNear = inNear; }
void Light::setFOV(float fov) { FOV = fov; }
void Light::setOrthoWidth(float orthoWidth) { this->orthoWidth = orthoWidth; }
void Light::setOrthoHeight(float orthoHeight) { this->orthoHeight = orthoHeight; }
void Light::setConstBias(float constBias) { this->constBias = constBias; }
void Light::setSlopeBias(float slopeBias) { this->slopeBias; }

float Light::getFar() const { return lightFar; }
float Light::getNear() const { return lightNear; }
float Light::getFOV() const { return FOV; }
float Light::getOrthoWidth() const { return orthoWidth; }
float Light::getOrthoHeight() const { return orthoHeight; }
float Light::getConstBias() const { return constBias; }
float Light::getSlopeBias() const { return slopeBias; }


bool Light::imGuiRender(size_t transformIncrement, bool& projectionChanged) {

	XMFLOAT3 lightPos = getGlobalPosition();
	XMFLOAT3 lightDir = getGlobalDirection();
	std::string posText = "Light Position: X: " + std::to_string(lightPos.x) + " Y: " + std::to_string(lightPos.y) + " Z: " + std::to_string(lightPos.z);
	std::string dirText = "Light Direction: X: " + std::to_string(lightDir.x) + " Y: " + std::to_string(lightDir.y) + " Z: " + std::to_string(lightDir.z);
	ImGui::Text(posText.c_str());
	ImGui::Text(dirText.c_str());
	bool lightUpdated = false;
	ImGui::Text(("Light Type: " + std::string(LightTypeStrings[type])).c_str());

	//Lights Diffuse Colour setting
	if (ImGui::TreeNode("Light Diffuse Colour")) {
		if (ImGui::ColorPicker4("Light Diffuse Colour: ", &diffuseColour.x)) { lightUpdated = true; }
		ImGui::TreePop();
	}

	if (m_transform.imGuiRender("Light Transform: ", transformIncrement, true, type != lightTypes::point, false))
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
	if (ImGui::TreeNode("Shadow Map Settings")) {
		if (ImGui::TreeNode("Projection Matrix Settings"))
		{
			if (ImGui::SliderFloat("Near", &lightNear, 0.01f, lightFar - 0.01f)) { projectionChanged = true; }
			if (ImGui::SliderFloat("Far", &lightFar, lightNear + 0.01f, 1000.f)) { projectionChanged = true; }

			if (type == lightTypes::directional)
			{
				if (ImGui::SliderFloat("Ortho Width", &orthoWidth, 1.f, 500.f)) { projectionChanged = true; }
				if (ImGui::SliderFloat("Ortho Height", &orthoHeight, 1.f, 500.f)) { projectionChanged = true; }
			}
			else if (type == lightTypes::spot)
			{
				if (ImGui::SliderAngle("FOV", &FOV, 5.f, 120.f)) { projectionChanged = true; }
			}
			ImGui::TreePop();
		}
		if (ImGui::SliderFloat("Constant Bias: ", &constBias, minConstBias, maxConstBias)) { lightUpdated = true; }
		if (ImGui::SliderFloat("Slope Bias: ", &slopeBias, minSlopeBias, maxSlopeBias)) { lightUpdated = true; }
		ImGui::TreePop();
	}
	
	return lightUpdated;
}


	

	

	
	

	

	