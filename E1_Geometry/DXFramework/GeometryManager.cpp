#include "GeometryManager.h"
#include "FileHandler.h"
#include "AModel.h"
#include "CubeMesh.h"
#include "Model.h"
#include "OrthoMesh.h"
#include "PlaneMesh.h"
#include "PointMesh.h"
#include "QuadMesh.h"
#include "SphereMesh.h"
#include "TessellationMesh.h"
#include "TriangleMesh.h"

GeometryManager::GeometryManager(ID3D11Device* device, ID3D11DeviceContext* deviceContext) : device(device), deviceContext(deviceContext) {}


std::shared_ptr<BaseMesh> GeometryManager::getMesh(const std::string& uid) {
	//Attempt retrieval from LRU cache
	std::shared_ptr<BaseMesh> retrievedMesh = meshLRU.get(uid);
	//If not in LRU cache
	if (retrievedMesh == nullptr) {
		auto it = meshMap.find(uid);
		//Attept retrieval from mesh map
		if (it != meshMap.end())
		{
			// mesh exists
			retrievedMesh = it->second;
			meshLRU.EmplaceReplace(uid, it->second);
		}
		else {
			return nullptr;
		}
	}
	return retrievedMesh;
}

std::string GeometryManager::generateUID(MeshType type) {
	// For now, just return the mesh type string as UID for no-parameter meshes
	switch (type) {
	case MeshType::POINT:
	case MeshType::QUAD:
	case MeshType::TESSELATION:
	case MeshType::TRIANGLE:
		return std::string(meshTypeStrings[static_cast<int>(type)]);
	default:
		throw std::runtime_error("Error, type must be Point, Quad, Tesselation or Triangle mesh!");
	}
}

std::string GeometryManager::generateUID(MeshType type, const std::string& file) {
	// For now, just return the mesh type string as UID for no-parameter meshes
	switch (type) {
	case MeshType::AMODEL:
	case MeshType::MODEL:
		return std::string(meshTypeStrings[static_cast<int>(type)]) + ": " + file;
	default:
		throw std::runtime_error("Error, type must be AModel or Model mesh!");
	}
}

std::string GeometryManager::generateUID(MeshType type, int resolution) {
	// For now, just return the mesh type string as UID for no-parameter meshes
	switch (type) {
	case MeshType::CUBE:
	case MeshType::PLANE:
	case MeshType::SPHERE:
		return std::string(meshTypeStrings[static_cast<int>(type)]) + ": " + std::to_string(resolution);
	default:
		throw std::runtime_error("Error, type must be Cube, Plane or Sphere mesh!");
	}
}
std::string GeometryManager::generateUID(MeshType type, int width, int height, int xPosition, int yPosition) {
	if (type != MeshType::ORTHO) 
	{
		throw std::runtime_error("Error, type must be Ortho mesh!");
		return ""; 
	}
	return std::string(meshTypeStrings[static_cast<int>(type)]) + ": " + std::to_string(width) + "_" + std::to_string(height) + "_" + std::to_string(xPosition) + "_" + std::to_string(yPosition);
}

std::shared_ptr<BaseMesh> GeometryManager::retrieveAdd(const std::string& uid, std::function<std::shared_ptr<BaseMesh>()> createFunc) {
	std::shared_ptr<BaseMesh> retrieval = getMesh(uid);
	if (!retrieval) {
		auto mesh = createFunc();
		addMesh(mesh, uid);
		return mesh;
	}
	return retrieval;
}

std::shared_ptr<BaseMesh> GeometryManager::createAModel(const std::string& file) {
	std::string uid = generateUID(MeshType::AMODEL, file);
	return retrieveAdd(uid, [&]() { return std::make_shared<AModel>(device, file); });
}

std::shared_ptr<BaseMesh> GeometryManager::createCubeMesh(int resolution) {
	std::string uid = generateUID(MeshType::CUBE, resolution);
	return retrieveAdd(uid, [&]() { return std::make_shared<CubeMesh>(device, deviceContext, resolution); });
}

std::shared_ptr<BaseMesh> GeometryManager::createModel(const std::string& file) {
	std::string uid = generateUID(MeshType::MODEL, file);
	return retrieveAdd(uid, [&]() { return std::make_shared<Model>(device, deviceContext, file.c_str()); });
}

std::shared_ptr<BaseMesh> GeometryManager::createOrthoMesh(int width, int height, int xPosition, int yPosition) {
	std::string uid = generateUID(MeshType::ORTHO, width, height, xPosition, yPosition);
	return retrieveAdd(uid, [&]() { return std::make_shared<OrthoMesh>(device, deviceContext, width, height, xPosition, yPosition); });
}

std::shared_ptr<BaseMesh> GeometryManager::createPlaneMesh(int resolution) {
	std::string uid = generateUID(MeshType::PLANE, resolution);
	return retrieveAdd(uid, [&]() { return std::make_shared<PlaneMesh>(device, deviceContext, resolution); });
}

std::shared_ptr<BaseMesh> GeometryManager::createPointMesh() {
	std::string uid = generateUID(MeshType::POINT);
	return retrieveAdd(uid, [&]() { return std::make_shared<PointMesh>(device, deviceContext); });
}

std::shared_ptr<BaseMesh> GeometryManager::createQuadMesh() {
	std::string uid = generateUID(MeshType::QUAD);
	return retrieveAdd(uid, [&]() { return std::make_shared<QuadMesh>(device, deviceContext); });
}

std::shared_ptr<BaseMesh> GeometryManager::createSphereMesh(int resolution) {
	std::string uid = generateUID(MeshType::SPHERE, resolution);
	return retrieveAdd(uid, [&]() { return std::make_shared<SphereMesh>(device, deviceContext, resolution); });
}

std::shared_ptr<BaseMesh> GeometryManager::createTesselationMesh() {
	std::string uid = generateUID(MeshType::TESSELATION);
	return retrieveAdd(uid, [&]() { return  std::make_shared<TessellationMesh>(device, deviceContext); });
}

std::shared_ptr<BaseMesh> GeometryManager::createTriangleMesh() {
	std::string uid = generateUID(MeshType::TRIANGLE);
	return retrieveAdd(uid, [&]() { return std::make_shared<TriangleMesh>(device, deviceContext); });
}

std::span<const char* const> GeometryManager::GetMeshTypeStrings() {
	return meshTypeStrings;
}

void GeometryManager::checkRemove(const std::string& uid) {

	// Try to get from LRU cache first
	std::shared_ptr<BaseMesh> mesh = meshLRU.get(uid);
	//Set internal refs to one for above local reference
	int internalRefs = 1;
	//In LRU cache also in map so add 2 to references
	if (mesh != nullptr) { internalRefs += 2; }
	else {

		auto it = meshMap.find(uid);
		//If in map increase internal refs and set TR
		if (it != meshMap.end())
		{
			mesh = it->second;
			++internalRefs;
		}
		else { return; }
	}
	//If use count only in manager class remove
	if (mesh.use_count() == internalRefs) {
		// No external references, safe to remove
		meshLRU.Remove(uid);
		meshMap.erase(uid);
	}
}

std::shared_ptr<BaseMesh> GeometryManager::addMesh(std::shared_ptr<BaseMesh> mesh, std::string uid) {
	auto pair = meshMap.insert(std::make_pair(uid, mesh));
	meshLRU.EmplaceReplace(uid, pair.first->second);
	return pair.first->second;
}