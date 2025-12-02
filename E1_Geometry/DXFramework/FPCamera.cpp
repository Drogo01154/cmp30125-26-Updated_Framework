// FPCamera class
// Represents a single First Person camera with basic movement.
#include "FPCamera.h"

// Configure defaul camera 
FPCamera::FPCamera(Input* in, int width, int height, HWND hnd)
{
	input = in;
	winWidth = width;
	winHeight = height;
	wnd = hnd;
}

void FPCamera::update() {
	//Calculate yaw rotation around Global up axis
	XMVECTOR outputRotation = XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), yaw);

	//Calculate updated local right vector from previous yaw rotation
	XMVECTOR localRight = XMVector3Rotate(XMVectorSet(1, 0, 0, 0), outputRotation);
	//Calculate pitch rotation around local yaw
	XMVECTOR pitchQ = XMQuaternionRotationAxis(localRight, pitch);

	//Multiply yaw and pitch rotation to get result 
	outputRotation = XMQuaternionMultiply(outputRotation, pitchQ);

	//Normalize before storing
	outputRotation = XMQuaternionNormalize(outputRotation);

	m_transform.setRotationQuat(outputRotation, false);

	Camera::update();
}

bool FPCamera::move(float dt)
{
	bool updatedCamera = false;
	setFrameTime(dt);
	// Handle the input.
	if (input->isKeyDown('W'))
	{
		// forward
		moveForward();
		updatedCamera = true;
	}
	if (input->isKeyDown('S'))
	{
		// back
		moveBackward();
		updatedCamera = true;
	}
	if (input->isKeyDown('A'))
	{
		// Strafe Left
		strafeLeft();
		updatedCamera = true;
	}
	if (input->isKeyDown('D'))
	{
		// Strafe Right
		strafeRight();
		updatedCamera = true;
	}
	if (input->isKeyDown('Q'))
	{
		// Down
		moveDownward();
		updatedCamera = true;
	}
	if (input->isKeyDown('E'))
	{
		// Up
		moveUpward();
		updatedCamera = true;
	}
	if (input->isKeyDown(VK_UP))
	{
		// rotate up
		turnUp();
		updatedCamera = true;
	}
	if (input->isKeyDown(VK_DOWN))
	{
		// rotate down
		turnDown();
		updatedCamera = true;
	}
	if (input->isKeyDown(VK_LEFT))
	{
		// rotate left
		turnLeft();
		updatedCamera = true;
	}
	if (input->isKeyDown(VK_RIGHT))
	{
		// rotate right
		turnRight();
		updatedCamera = true;
	}

	if (input->isMouseActive())
	{
		// mouse look is on
		deltax = input->getMouseX() - (winWidth / 2);
		deltay = input->getMouseY() - (winHeight / 2);
		turn(deltax, deltay);
		cursor.x = winWidth / 2;
		cursor.y = winHeight / 2;
		ClientToScreen(wnd, &cursor);
		SetCursorPos(cursor.x, cursor.y);
		updatedCamera = true;
	}

	if (input->isRightMouseDown() && !input->isMouseActive())
	{
		// re-position cursor
		cursor.x = winWidth / 2;
		cursor.y = winHeight / 2;
		ClientToScreen(wnd, &cursor);
		SetCursorPos(cursor.x, cursor.y);
		input->setMouseX(winWidth / 2);
		input->setMouseY(winHeight / 2);
		
		// set mouse tracking as active and hide mouse cursor
		input->setMouseActive(true);
		ShowCursor(false);
	}
	else if (!input->isRightMouseDown() && input->isMouseActive())
	{
		// disable mouse tracking and show mouse cursor
		input->setMouseActive(false);
		ShowCursor(true);
	}

	//if (input->isKeyDown(VK_SPACE))
	//{
	//	// re-position cursor
	//	cursor.x = winWidth / 2;
	//	cursor.y = winHeight / 2;
	//	ClientToScreen(wnd, &cursor);
	//	SetCursorPos(cursor.x, cursor.y);
	//	input->setMouseX(winWidth / 2);
	//	input->setMouseY(winHeight / 2);
	//	input->SetKeyUp(VK_SPACE);
	//	// if space pressed toggle mouse
	//	input->setMouseActive(!input->isMouseActive());
	//	if (!input->isMouseActive())
	//	{
	//		ShowCursor(true);
	//	}
	//	else
	//	{
	//		ShowCursor(false);
	//	}
	//}
	update();
	return updatedCamera;
}