#pragma once
// Geometry
// Loads and stores meshes ready for rendering
// Stores geometry instances in a geometry cache

#ifndef _GEOMETRYMANAGER_H_
#define _GEOMETRYMANAGER_H_

#include "BaseMesh.h"
#include "LRUCache.h"
#include <array>
#include <string>
#include <functional>
#include <span>
#include <unordered_set>

enum class MeshType : uint8_t
{
	AMODEL,				// String Paramater
	MODEL,				// String Paramater
	POINT,			// No Paramater
	QUAD,			// No Paramater
	TESSELATION,	// No Paramater
	TRIANGLE,			// No Paramater
	CUBE,				// int Paramater
	PLANE,				// int paramater
	SPHERE,				// int paramater
	ORTHO				// four int paramater
};

struct MeshData {
	MeshType type;
	std::shared_ptr<BaseMesh> mesh;
};

class GeometryManager {
public:
	GeometryManager(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	std::string generateUID(MeshType type);
	std::string generateUID(MeshType type, const std::string& file);
	std::string generateUID(MeshType type, int resolution);
	std::string generateUID(MeshType type, int width, int height, int xPosition, int yPosition);

	std::shared_ptr<BaseMesh> getMesh(const std::string& uid);

	std::shared_ptr<BaseMesh> createAModel(const std::string& file);
	std::shared_ptr<BaseMesh> createCubeMesh(int resolution = 20);
	std::shared_ptr<BaseMesh> createModel(const std::string& filename);
	std::shared_ptr<BaseMesh> createOrthoMesh(int width, int height, int xPosition = 0, int yPosition = 0);
	std::shared_ptr<BaseMesh> createPlaneMesh(int resolution = 20);
	std::shared_ptr<BaseMesh> createPointMesh();
	std::shared_ptr<BaseMesh> createQuadMesh();
	std::shared_ptr<BaseMesh> createSphereMesh(int resolution = 20);
	std::shared_ptr<BaseMesh> createTesselationMesh();
	std::shared_ptr<BaseMesh> createTriangleMesh();

	void checkRemove(const std::string& uid); //Unloads mesh if no more in scene
	 
	std::span<const char* const> GetMeshTypeStrings();

private:
	std::shared_ptr<BaseMesh> retrieveAdd(const std::string& uid, std::function<std::shared_ptr<BaseMesh>()> createFunc);
	std::shared_ptr<BaseMesh> addMesh(std::shared_ptr<BaseMesh>, std::string uid);

	inline static const char* meshTypeStrings[10] = {
		"AModel",
		"Model",
		"Point",
		"Quad",
		"Tesselation",
		"Triangle",
		"Cube",
		"Plane",
		"Sphere",
		"Ortho"
	};

	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	LRUCache<std::string, std::shared_ptr<BaseMesh>> meshLRU;
	std::unordered_map<std::string, std::shared_ptr<BaseMesh>> meshMap;
};

#endif