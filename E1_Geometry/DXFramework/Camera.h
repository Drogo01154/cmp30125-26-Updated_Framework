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

using namespace DirectX;

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
	~Camera();

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

	void ImGuiRender(size_t transformIncrement);

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

	Transform m_transform;

private:
	XMFLOAT3 globalPosition;		///< float3 for position
	XMFLOAT3 globalDirection;		///< float3 for rotation (angles)
	XMMATRIX viewMatrix;	///< matrix for current view
	XMMATRIX orthoMatrix;	///< current orthographic matrix
	float speed, frameTime;	///< movement speed and time variables
	float lookSpeed;		///< rotation speed
};


namespace nlohmann {
	template<>
	struct adl_serializer<Camera> {
		static void to_json(json& j, const Camera& c) {
			j["transform"] = c.m_transform;
			j["speed"] = c.getSpeed();
			j["lookSpeed"] = c.getLookSpeed();
		}

		static void from_json(const json& j, Camera& c) {
			c.m_transform = j.at("transform").get<Transform>();
			c.setSpeed(j.at("speed").get<float>());
			c.setLookSpeed(j.at("lookSpeed").get<float>());
		}
	};
}
#endif