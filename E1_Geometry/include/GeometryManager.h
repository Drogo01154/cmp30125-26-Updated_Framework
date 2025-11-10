#pragma once
// Geometry
// Loads and stores meshes ready for rendering
// Stores geometry instances in a geometry cache

#ifndef _GEOMETRYMANAGER_H_
#define _GEOMETRYMANAGER_H_

#include "BaseMesh.h"
#include "LRUCache.h"
#include <string>

enum class MeshType
{
	AModel,
	Cube,
	Model,
	Ortho,
	Plane,
	Point,
	Quad,
	Sphere,
	Tesselation,
	Triangle
};

class GeometryManager {
public:
	GeometryManager(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	~GeometryManager();

	std::shared_ptr<BaseMesh> getMesh(const std::string& uid);

	std::shared_ptr<BaseMesh> createAModel(const std::string& file);
	std::shared_ptr<BaseMesh> createCubeMesh(int resolution = 20);
	std::shared_ptr<BaseMesh> createModel(const char* filename);
	std::shared_ptr<BaseMesh> createOrthoMesh(int width, int height, int xPosition = 0, int yPosition = 0);
	std::shared_ptr<BaseMesh> createPlaneMesh(int resolution = 20);
	std::shared_ptr<BaseMesh> createPointMesh();
	std::shared_ptr<BaseMesh> createQuadMesh();
	std::shared_ptr<BaseMesh> createSphereMesh(int resolution = 20);
	std::shared_ptr<BaseMesh> createTesselationMesh();
	std::shared_ptr<BaseMesh> createTriangleMesh();


private:

	void addMesh(const std::string& uid, std::shared_ptr<BaseMesh>);
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	LRUCache<std::string, std::shared_ptr<BaseMesh>> meshLRU;
	std::unordered_map<std::string, std::shared_ptr<BaseMesh>> meshMap;
};

#endif