// Camera class
// Represents a single 3D camera with basic movement.
#include "camera.h"

// Configure defaul camera (including positions, rotation and ortho matrix)
Camera::Camera()
{
	lookSpeed = 4.0f;

	// Generate ortho matrix
	XMVECTOR up, position, lookAt;
	up = XMVectorSet(0.0f, 1.0, 0.0, 1.0f);
	position = XMVectorSet(0.0f, 0.0, -10.0, 1.0f);
	lookAt = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0);
	orthoMatrix = XMMatrixLookAtLH(position, lookAt, up);
	yaw = 0; pitch = 0;
}

//Getters
float Camera::getLookSpeed() const { return lookSpeed; }
float Camera::getYaw() const { return yaw; }
float Camera::getPitch() const { return pitch; }
const XMFLOAT3& Camera::getGlobalPosition() const {
	return globalPosition;
}
const XMFLOAT3& Camera::getGlobalDirection() const {
	return globalDirection;
}

XMMATRIX Camera::getViewMatrix()
{
	return viewMatrix;
}

XMMATRIX Camera::getOrthoViewMatrix()
{
	return orthoMatrix;
}


//Setters
void Camera::setLookSpeed(float speed) { this->lookSpeed = speed; }
void Camera::setYaw(float yaw) { this->yaw = yaw; }
void Camera::setPitch(float pitch) { this->pitch = pitch; }

//ImGui Menu
bool Camera::imGuiRender(size_t transformIterator, CameraTypes type) {
	bool cameraUpdated = false;
	std::string outputPos = "Camera Position: X: " + std::to_string(globalPosition.x) + " Y: " + std::to_string(globalPosition.y) + " Z: " + std::to_string(globalPosition.z);
	XMFLOAT3 dir = getGlobalDirection();
	std::string outputDir = "Camera Direction: X: " + std::to_string(dir.x) + " Y: " + std::to_string(dir.y) + " Z: " + std::to_string(dir.z);
	ImGui::Text(outputPos.c_str());
	ImGui::Text(outputDir.c_str());

	switch (type) {
	case CameraTypes::BASIC:
		if (m_transform.imGuiRender("Camera Transform", transformIterator, true, true, false)) {
			updateGlobals(true);
			cameraUpdated = true;
		}
		break;
	case CameraTypes::FPCAMERA:
		if(ImGui::SliderFloat("Look Speed", &lookSpeed, 0.1f, 100.f)) {}
		break;
	}
	return cameraUpdated;
}

// Store frame/delta time.
void Camera::setFrameTime(float t)
{
	frameTime = t;
}

//Function for updating camera global transform values
void Camera::updateGlobals(bool updateTransform) {
	if (updateTransform) { m_transform.computeGlobalMatrix(true); }
	XMMATRIX globalMatrix = m_transform.getGlobalMatrix();
	XMVECTOR outTranslation, outRotation, outScale;
	XMMatrixDecompose(&outScale, &outRotation, &outTranslation, globalMatrix);
	XMStoreFloat3(&globalPosition, outTranslation);
	XMStoreFloat3(&globalDirection, m_transform.getWorldForward());
}

// Re-calucation view Matrix.
void Camera::update()
{
	updateGlobals(true);
	XMVECTOR up, positionv, lookAt;
	XMMATRIX rotationMatrix;
	
	//Set up viedw vectors
	positionv = XMLoadFloat3(&globalPosition);
	lookAt = XMLoadFloat3(&globalDirection);
	up = m_transform.getWorldUp();
	//Translate rotation of camera to its position
	lookAt = positionv + lookAt;

	//Create look at matrix from vectors
	viewMatrix = XMMatrixLookAtLH(positionv, lookAt, up);
}

void Camera::moveForward()
{
	speed = frameTime * 5.0f;

	// Get forward vector from local transform
	XMVECTOR forward = m_transform.getWorldForward();
	forward = XMVector3Normalize(forward);

	// Scale by speed
	XMVECTOR delta = forward * speed;

	// Apply translation
	m_transform.translate(delta, false);
}


void Camera::moveBackward()
{
	speed = frameTime * 5.0f;

	// Get forward vector from local transform
	XMVECTOR forward = m_transform.getWorldForward();

	// Scale by speed
	XMVECTOR delta = -forward * speed;

	// Apply translation
	m_transform.translate(delta, false);
}


void Camera::moveUpward()
{
	speed = frameTime * 5.0f;

	// Get up vector from local transform
	XMVECTOR up = m_transform.getWorldUp();

	// Scale by speed
	XMVECTOR delta = up * speed;

	// Apply translation
	m_transform.translate(delta, false);
}


void Camera::moveDownward()
{
	speed = frameTime * 5.0f;

	// Get up vector from local transform
	XMVECTOR up = m_transform.getWorldUp();

	// Scale by speed
	XMVECTOR delta = -up * speed;

	// Apply translation
	m_transform.translate(delta, false);
}


void Camera::turnLeft()
{
	// Update the left turn movement based on the frame time 
	speed = frameTime * 25.0f * 0.0174533f;
	yaw -= speed;
}


void Camera::turnRight()
{
	// Update the left turn movement based on the frame time 
	speed = frameTime * 25.0f * 0.0174533f;
	yaw += speed;
}


void Camera::turnUp()
{
	// Update the left turn movement based on the frame time 
	speed = frameTime * 25.0f * 0.0174533f;
	pitch += speed;
}


void Camera::turnDown()
{
	// Update the left turn movement based on the frame time 
	speed = frameTime * 25.0f * 0.0174533f;

	pitch -= speed;
}


void Camera::turn(int x, int y)
{
	float deg2rad = 0.0174533f;
	pitch += float(y) / lookSpeed * deg2rad;
	yaw += float(x) / lookSpeed * deg2rad;
}

void Camera::strafeRight()
{
	speed = frameTime * 5.0f;

	// Get right vector from local transform
	XMVECTOR right = m_transform.getWorldRight();

	// Scale by speed
	XMVECTOR delta = right * speed;

	// Apply translation
	m_transform.translate(delta, false);
}

void Camera::strafeLeft()
{
	speed = frameTime * 5.0f;

	// Get up vector from local transform
	XMVECTOR right = m_transform.getWorldRight();

	// Scale by speed
	XMVECTOR delta = -right * speed;

	// Apply translation
	m_transform.translate(delta, false);
}

std::span<const char* const> Camera::GetCameraTypeStrings() {
	return CameraTypeStrings;
}