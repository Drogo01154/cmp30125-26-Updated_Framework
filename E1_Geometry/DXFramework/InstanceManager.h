#pragma once

#ifndef _INSTANCEMANAGER_H_
#define _INSTANCEMANAGER_H_

#include "Light.h"
#include "FPCamera.h"
#include "MaterialManager.h"
#include "GeometryManager.h"
#include "InstanceCache.h"
#include <variant>
#include "Input.h"

using ParamValue = std::variant<int, std::string>;

struct GeometryInstance {

	Transform m_transform;									// Local Transform
	
	std::shared_ptr<BaseMesh> mesh = nullptr;				// Runtime only basemesh ptr
	MeshType type;											// Stored type for serialization
	std::unordered_map<std::string, ParamValue> params;		// Stored paramaters for serialization

	std::shared_ptr<Material> mat = nullptr;				// Runtime only matertial ptr
	size_t materialID;										// Material ID for serializaton
};

struct CameraInstance {
	std::shared_ptr<Camera> camera;
	CameraTypes type;
};



class InstanceManager {
public:
	InstanceManager(GeometryManager* geometryManager, MaterialManager* materialManager, Input* input, HWND hwnd, int screenWidth, int screenHeight);
	std::weak_ptr<GeometryInstance> getGeometryInstance(size_t ID);
	std::weak_ptr<Light> getLightInstance(size_t ID);
	std::weak_ptr<CameraInstance> getCameraInstance(size_t ID);

	std::string buildMeshUID(std::weak_ptr<GeometryInstance> instance);

	void removeGeometryInstance(size_t ID);
	void removeLightInstance(size_t ID);
	void removeCameraInstance(size_t ID);

	size_t getNumberOfLights();
	size_t getNumberOfMeshes();
	size_t getNumberOfCameras();

	void clearGeometry();
	void clearLights();
	void clearCameras();
	void clearAll();

	//Create Geometry
	std::weak_ptr<GeometryInstance> createAModelInstance(size_t& instanceID, const std::string& file);
	std::weak_ptr<GeometryInstance> createCubeMeshInstance(size_t& instanceID, int resolution = 20);
	std::weak_ptr<GeometryInstance> createModelInstance(size_t& instanceID, const std::string& filename);
	std::weak_ptr<GeometryInstance> createOrthoMeshInstance(size_t& instanceID, int width, int height, int xPosition = 0, int yPosition = 0);
	std::weak_ptr<GeometryInstance> createPlaneMeshInstance(size_t& instanceID, int resolution = 20);
	std::weak_ptr<GeometryInstance> createPointMeshInstance(size_t& instanceID);
	std::weak_ptr<GeometryInstance> createQuadMeshInstance(size_t& instanceID);
	std::weak_ptr<GeometryInstance> createSphereMeshInstance(size_t& instanceID, int resolution = 20);
	std::weak_ptr<GeometryInstance> createTesselationMeshInstance(size_t& instanceID);
	std::weak_ptr<GeometryInstance> createTriangleMeshInstance(size_t& instanceID);

	//Create Lights
	std::pair<size_t, std::shared_ptr<Light>>  createLight(lightTypes type);

	//Create Cameras
	std::pair<size_t, std::shared_ptr<CameraInstance>>  createCamera();
	std::pair<size_t, std::shared_ptr<CameraInstance>>  createFPCamera();

	std::shared_ptr<CameraInstance> getActiveCamera();
	void setActiveCamera(size_t ID);

	void to_json(nlohmann::json& j);
	void from_json(const nlohmann::json& j,
		std::unordered_map<size_t, size_t>* newCameraIDMap,
		std::unordered_map<size_t, size_t>* newMeshIDMap,
		std::unordered_map<size_t, size_t>* newLightIDMap);

	template<typename Func>
	inline void forEachMesh(Func&& func) const {
		geometryInstances.forEach(std::forward<Func>(func));
	}

	template<typename Func>
	inline void forEachLight(Func&& func) const {
		lightInstances.forEach(std::forward<Func>(func));
	}

	template<typename Func>
	inline void forEachCamera(Func&& func) const {
		cameraInstances.forEach(std::forward<Func>(func));
	}

private:
	std::shared_ptr<GeometryInstance> createGeometryInstance(size_t& instanceID, MeshType type, std::shared_ptr<BaseMesh> newMesh);

	Input* input; 
	HWND hwnd; 
	int screenWidth; 
	int screenHeight;

	size_t activeCameraID;
	std::shared_ptr<CameraInstance> activeCamera;

	GeometryManager* geometryManager;
	MaterialManager* materialManager;
	IDMap<GeometryInstance> geometryInstances;
	IDMap<Light> lightInstances;
	IDMap<CameraInstance> cameraInstances;
};

#endif