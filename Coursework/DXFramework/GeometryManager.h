#pragma once
// Geometry
// Loads and stores meshes ready for rendering
// Stores geometry instances in a geometry cache

#ifndef _GEOMETRYMANAGER_H_
#define _GEOMETRYMANAGER_H_

#include "BaseMesh.h"
#include "LRUCache.h"
#include "InstanceCache.h"
#include <array>
#include <span>
#include <unordered_set>
#include <optional>
#include <variant>

enum class MeshType : uint8_t
{
	AMODEL,				// String Paramater
	MODEL,				// String Paramater
	POINT,				// No Paramater
	QUAD,				// No Paramater
	TESSELATION,		// No Paramater
	TRIANGLE,			// No Paramater
	CUBE,				// int Paramater
	PLANE,				// int paramater
	SPHERE,				// int paramater
	ORTHO				// four int paramater
};

struct OrthoMeshData;

using ParamValue = std::variant<int, std::string>;

struct MeshData {
	MeshData(MeshType type, std::shared_ptr<BaseMesh> mesh,
		std::unordered_map<std::string, ParamValue> params = {})
		: type(type), mesh(mesh), params(std::move(params)) {
	}
	MeshType type;
	std::unordered_map<std::string, ParamValue> params;		// Stored paramaters for serialization
	std::shared_ptr<BaseMesh> mesh;
};

#define MeshInstance Instance<std::string, MeshData>

/*
	Geometry manager class for caching meshes for resuse.
*/
class GeometryManager {
public:
	GeometryManager(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	std::string generateUID(MeshType type,
		std::optional<std::string> file = std::nullopt,
		std::optional<int> resolution = std::nullopt,
		std::optional<int> width = std::nullopt,
		std::optional<int> height = std::nullopt,
		std::optional<int> xPos = std::nullopt,
		std::optional<int> yPos = std::nullopt);


	MeshInstance tryGetMesh(const std::string& uid);

	MeshInstance createAModel(const std::string& file);
	MeshInstance createCubeMesh(int resolution = 20);
	MeshInstance createModel(const std::string& filename);
	MeshInstance createOrthoMesh(int width, int height, int xPosition = 0, int yPosition = 0);
	MeshInstance createOrthoMesh(const OrthoMeshData orthoData);
	MeshInstance createPlaneMesh(int resolution = 20);
	MeshInstance createPointMesh();
	MeshInstance createQuadMesh();
	MeshInstance createSphereMesh(int resolution = 20);
	MeshInstance createTesselationMesh();
	MeshInstance createTriangleMesh();

	std::string determineMeshUID(MeshData* data);

	void setDestroyNoInstances(bool value);

	std::span<const char* const> GetMeshTypeStrings();

private:

	MeshInstance RetrieveAddMesh(MeshType type,
		std::optional<std::string> file = std::nullopt,
		std::optional<int> resolution = std::nullopt,
		std::optional<int> width = std::nullopt,
		std::optional<int> height = std::nullopt,
		std::optional<int> xPos = std::nullopt,
		std::optional<int> yPos = std::nullopt);

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
	InstanceCache<std::string, MeshData> meshCache;
};

#endif