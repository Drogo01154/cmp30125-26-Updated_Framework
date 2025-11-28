/**
* \class Camera
*
* \brief Creates and maintains several matrices for 3D viewing.
*
* Maintains both prospective and orthographic view matrices.
* With functions for moving and rotating the camera/matrices.
*
* \author Paul Robertson
*/

#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "transform.h"
#include <span>

using namespace DirectX;

enum CameraTypes : int {
	BASIC,
	FPCAMERA
};

class Camera
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

	Camera();	///< Initialised default camera object

	virtual ~Camera() = default;

	XMFLOAT3 getGlobalPosition() const;		///< Get camera's current position
	XMFLOAT3 getGlobalDirection() const;		///< Get camera's current rotation

	void updateGlobals(bool updateTransform);

	void update();				///< Update camera, recalculates view matrix based on rotation
	XMMATRIX getViewMatrix();	///< Get current view matrix of camera
	XMMATRIX getOrthoViewMatrix();	///< Get current orthographic view matrix for camera

	void setFrameTime(float);
	
	float getSpeed() const;
	float getLookSpeed() const;

	void setSpeed(float speed);
	void setLookSpeed(float speed);

	bool imGuiRender(size_t transformIterator, CameraTypes type);

	void moveForward();			///< default function for moving forward
	void moveBackward();		///< default function for moving backward
	void moveUpward();			///< default function for moving upward
	void moveDownward();		///< default function for moving downward
	void turnLeft();			///< default function for turning left
	void turnRight();			///< default function for turning right
	void turnUp();				///< default function for looking up
	void turnDown();			///< default function for looking down
	void strafeRight();			///< default function for moving right
	void strafeLeft();			///< default function for moving left
	void turn(int x, int y);	///< default function for turning in both x/y axis

	static std::span<const char* const> GetCameraTypeStrings();

	Transform m_transform;

private:
	XMFLOAT3 globalPosition;		///< float3 for position
	XMFLOAT3 globalDirection;		///< float3 for rotation (angles)
	XMMATRIX viewMatrix;	///< matrix for current view
	XMMATRIX orthoMatrix;	///< current orthographic matrix
	float speed, frameTime;	///< movement speed and time variables
	float lookSpeed;		///< rotation speed

	inline static const char* CameraTypeStrings[3] =
	{
		"Basic",
		"First Person",
	};
};
#endif