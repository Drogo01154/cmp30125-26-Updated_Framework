#pragma once

#ifndef _INSTANCEMANAGER_H_
#define _INSTANCEMANAGER_H_

#include "Light.h"
#include "FPCamera.h"
#include "MaterialManager.h"
#include "GeometryManager.h"
#include "InstanceCache.h"
#include "Input.h"

struct GeometryData {

	Transform m_transform;	// Local Transform

	MeshInstance mesh;		// Base Mesh instance
	MaterialInstance mat;	// Material Instance
};

struct CameraData {
	CameraData(CameraTypes type, std::shared_ptr<Camera> camera) : type(type), camera(camera) {}
	std::shared_ptr<Camera> camera;
	CameraTypes type;
};

#define GeometryInstance Instance<size_t, GeometryData>
#define LightInstance Instance<size_t, Light>
#define CameraInstance Instance<size_t, CameraData>

struct OrthoMeshData;

class InstanceManager {
public:
	InstanceManager(
		ShaderManager* shaderManager, 
		GeometryManager* geometryManager, 
		MaterialManager* materialManager, 
		Input* input, HWND hwnd, 
		int screenWidth, int screenHeight);
	GeometryInstance tryGetGeometryInstance(size_t ID);
	LightInstance tryGetLightInstance(size_t ID);
	CameraInstance tryGetCameraInstance(size_t ID);

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
	GeometryInstance createAModelInstance(size_t& instanceID, const std::string& file);
	GeometryInstance createCubeMeshInstance(size_t& instanceID, int resolution = 20);
	GeometryInstance createModelInstance(size_t& instanceID, const std::string& filename);
	GeometryInstance createOrthoMeshInstance(size_t& instanceID, int width, int height, int xPosition = 0, int yPosition = 0);
	GeometryInstance createOrthoMeshInstance(size_t& instanceID, const OrthoMeshData& orthoData);
	GeometryInstance createPlaneMeshInstance(size_t& instanceID, int resolution = 20);
	GeometryInstance createPointMeshInstance(size_t& instanceID);
	GeometryInstance createQuadMeshInstance(size_t& instanceID);
	GeometryInstance createSphereMeshInstance(size_t& instanceID, int resolution = 20);
	GeometryInstance createTesselationMeshInstance(size_t& instanceID);
	GeometryInstance createTriangleMeshInstance(size_t& instanceID);

	//Create Lights
	LightInstance createLight(size_t& instanceID, lightTypes type);

	//Create Cameras
	CameraInstance  createCamera(size_t& instanceID);
	CameraInstance  createFPCamera(size_t& instanceID);

	size_t getActiveCameraID();
	CameraData* getActiveCamera();
	void setActiveCamera(size_t ID);

	void to_json(nlohmann::json& j);
	void from_json(const nlohmann::json& j,
		std::unordered_map<size_t, size_t>* newCameraIDMap,
		std::unordered_map<size_t, size_t>* newMeshIDMap,
		std::unordered_map<size_t, size_t>* newLightIDMap);

	template<typename Func>
	inline void forEachMesh(Func&& func) {
		geometryCache.forEach(std::forward<Func>(func));
	}

	template<typename Func>
	inline void forEachLight(Func&& func) {
		lightCache.forEach(std::forward<Func>(func));
	}

	template<typename Func>
	inline void forEachCamera(Func&& func) {
		cameraCache.forEach(std::forward<Func>(func));
	}
	void setDestroyNoInstances(bool value);

private:
	GeometryInstance createGeometryInstance(size_t& instanceID, MeshInstance mesh, bool addMaterial = true);

	Input* input; 
	HWND hwnd; 
	int screenWidth; 
	int screenHeight;

	size_t activeCameraID;
	std::shared_ptr<CameraInstance> activeCamera;

	ShaderManager* shaderManager;
	GeometryManager* geometryManager;
	MaterialManager* materialManager;
	InstanceCache<size_t, GeometryData> geometryCache;
	InstanceCache<size_t, Light> lightCache;
	InstanceCache<size_t, CameraData> cameraCache;
};

#endif