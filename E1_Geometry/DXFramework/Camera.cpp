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
}

float Camera::getSpeed() const { return speed; }
float Camera::getLookSpeed() const { return lookSpeed; }

void Camera::setSpeed(float speed) { this->speed = speed; }
void Camera::setLookSpeed(float speed) { this->lookSpeed = speed; }

XMFLOAT3 Camera::getGlobalPosition() const {
	return globalPosition;
}
XMFLOAT3 Camera::getGlobalDirection() const {
	return globalDirection;
}

bool Camera::imGuiRender(size_t transformIterator, CameraTypes type) {
	bool cameraUpdated = false;
	std::string outputPos = "X: " + std::to_string(globalPosition.x) + " Y: " + std::to_string(globalPosition.y) + " Z: " + std::to_string(globalPosition.z);
	ImGui::Text(outputPos.c_str());

	switch (type) {
	case CameraTypes::BASIC:
		if (m_transform.imGuiRender("Camera Transform", transformIterator, true, true, false)) {
			updateGlobals(true);
			cameraUpdated = true;
		}
		break;
	case CameraTypes::FPCAMERA:
		if(ImGui::SliderFloat("Speed", &speed, 0.1f, 100.f)) {}
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
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;
	
	// Setup the vectors
	up = XMVectorSet(0.0f, 1.0, 0.0, 1.0f);
	positionv = XMLoadFloat3(&globalPosition);
	lookAt = XMVectorSet(0.0, 0.0, 1.0f, 1.0f);
	
	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	pitch = globalDirection.x * 0.0174532f;
	yaw = globalDirection.y * 0.0174532f;
	roll = globalDirection.z * 0.0174532f;

	// Create the rotation matrix from the yaw, pitch, and roll values.
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	lookAt = XMVector3TransformCoord(lookAt, rotationMatrix);
	up = XMVector3TransformCoord(up, rotationMatrix);
	
	// Translate the rotated camera position to the location of the viewer.
	lookAt = positionv + lookAt;

	// Finally create the view matrix from the three updated vectors.
	viewMatrix = XMMatrixLookAtLH(positionv, lookAt, up);
}


XMMATRIX Camera::getViewMatrix()
{
	return viewMatrix;
}

XMMATRIX Camera::getOrthoViewMatrix()
{
	return orthoMatrix;
}

void Camera::moveForward()
{
	float speed = frameTime * 5.0f;

	// Get forward vector from local transform
	XMVECTOR forward = m_transform.getWorldForward(); // Z axis
	forward = XMVector3Normalize(forward);

	// Scale by speed
	XMVECTOR delta = forward * speed;

	// Apply translation
	m_transform.translate(delta);
}


void Camera::moveBackward()
{
	float speed = frameTime * 5.0f;

	// Get forward vector from local transform
	XMVECTOR forward = m_transform.getWorldForward(); // Z axis

	// Scale by speed
	XMVECTOR delta = -forward * speed;

	// Apply translation
	m_transform.translate(delta);
}


void Camera::moveUpward()
{
	float speed = frameTime * 5.0f;

	// Get up vector from local transform
	XMVECTOR up = m_transform.getWorldUp(); // Y axis

	// Scale by speed
	XMVECTOR delta = up * speed;

	// Apply translation
	m_transform.translate(delta);
}


void Camera::moveDownward()
{
	float speed = frameTime * 5.0f;

	// Get up vector from local transform
	XMVECTOR up = m_transform.getWorldUp(); // Y axis

	// Scale by speed
	XMVECTOR delta = -up * speed;

	// Apply translation
	m_transform.translate(delta);
}


void Camera::turnLeft()
{
	// Update the left turn movement based on the frame time 
	speed = frameTime * 25.0f;
	
	m_transform.rotate(XMFLOAT3(0.0f, -speed, 0.0f));
}


void Camera::turnRight()
{
	// Update the left turn movement based on the frame time 
	speed = frameTime * 25.0f;

	m_transform.rotate(XMFLOAT3(0.0f, speed, 0.0f));
}


void Camera::turnUp()
{
	// Update the left turn movement based on the frame time 
	speed = frameTime * 25.0f;

	m_transform.rotate(XMFLOAT3(speed, 0.0f, 0.0f));
}


void Camera::turnDown()
{
	// Update the left turn movement based on the frame time 
	speed = frameTime * 25.0f;

	m_transform.rotate(XMFLOAT3(-speed, 0.0f, 0.0f));
}


void Camera::turn(int x, int y)
{
	m_transform.rotate(XMFLOAT3((float)x / lookSpeed, (float)y / lookSpeed, 0.0f));
}

void Camera::strafeRight()
{
	float speed = frameTime * 5.0f;

	// Get up vector from local transform
	XMVECTOR right = m_transform.getWorldRight(); // Y axis

	// Scale by speed
	XMVECTOR delta = right * speed;

	// Apply translation
	m_transform.translate(delta);
}

void Camera::strafeLeft()
{
	float speed = frameTime * 5.0f;

	// Get up vector from local transform
	XMVECTOR right = m_transform.getWorldRight(); // Y axis

	// Scale by speed
	XMVECTOR delta = -right * speed;

	// Apply translation
	m_transform.translate(delta);
}
std::span<const char* const> Camera::GetCameraTypeStrings() {
	return CameraTypeStrings;
}