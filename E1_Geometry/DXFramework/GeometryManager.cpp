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
	if (type == MeshType::Point || type == MeshType::Quad || type == MeshType::Tesselation || type == MeshType::Triangle) {
		return std::string(meshTypeStrings[static_cast<int>(type)]);
	}
	else {
		throw std::runtime_error("Error, type must be Point, Quad, Tesselation or Trinagle mesh!");
		return "";
	}
}

std::string GeometryManager::generateUID(MeshType type, const std::string& file) {
	
	if (type == MeshType::AModel || type == MeshType::Model) {
		return std::string(meshTypeStrings[static_cast<int>(type)]) + ": " + file;
	}
	else {
		throw std::runtime_error("Error, type must be Point, Quad, Tesselation or Trinagle mesh!");
		return "";
	}
}

std::string GeometryManager::generateUID(MeshType type, int resolution) {
	if (type == MeshType::Cube || type == MeshType::Plane || type == MeshType::Sphere)
	{
		return std::string(meshTypeStrings[static_cast<int>(type)]) + ": " + std::to_string(resolution);
	}
	else {
		throw std::runtime_error("Error, type must be Cube, Plane or Sphere mesh!");
		return "";
	}
}
std::string GeometryManager::generateUID(MeshType type, int width, int height, int xPosition, int yPosition) {
	if (type != MeshType::Ortho) 
	{
		throw std::runtime_error("Error, type must be ortho mesh!");
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
	std::string uid = generateUID(MeshType::AModel, file);
	return retrieveAdd(uid, [&]() { return std::make_shared<AModel>(device, file); });
}

std::shared_ptr<BaseMesh> GeometryManager::createCubeMesh(int resolution) {
	std::string uid = generateUID(MeshType::Cube, resolution);
	return retrieveAdd(uid, [&]() { return std::make_shared<CubeMesh>(device); });
}

std::shared_ptr<BaseMesh> GeometryManager::createModel(const std::string& file) {
	std::string uid = generateUID(MeshType::Model, file);
	return retrieveAdd(uid, [&]() { return std::make_shared<Model>(device, file.c_str()); });
}

std::shared_ptr<BaseMesh> GeometryManager::createOrthoMesh(int width, int height, int xPosition, int yPosition) {
	std::string uid = generateUID(MeshType::Ortho, width, height, xPosition, yPosition);
	return retrieveAdd(uid, [&]() { return std::make_shared<OrthoMesh>(device, deviceContext, width, height, xPosition, yPosition) });
}

std::shared_ptr<BaseMesh> GeometryManager::createPlaneMesh(int resolution) {
	std::string uid = generateUID(MeshType::Plane, resolution);
	return retrieveAdd(uid, [&]() { return std::make_shared<PlaneMesh>(device, deviceContext, resolution); });
}

std::shared_ptr<BaseMesh> GeometryManager::createPointMesh() {
	std::string uid = generateUID(MeshType::Point);
	return retrieveAdd(uid, [&]() { return std::make_shared<PointMesh>(device); });
}

std::shared_ptr<BaseMesh> GeometryManager::createQuadMesh() {
	std::string uid = generateUID(MeshType::Quad);
	return retrieveAdd(uid, [&]() { return std::make_shared<QuadMesh>(device); });
}

std::shared_ptr<BaseMesh> GeometryManager::createSphereMesh(int resolution) {
	std::string uid = generateUID(MeshType::Sphere, resolution);
	return retrieveAdd(uid, [&]() { return std::make_shared<SphereMesh>(device, deviceContext, resolution); });
}

std::shared_ptr<BaseMesh> GeometryManager::createTesselationMesh() {
	std::string uid = generateUID(MeshType::Tesselation);
	return retrieveAdd(uid, [&]() { return  std::make_shared<TessellationMesh>(device, deviceContext); });
}

std::shared_ptr<BaseMesh> GeometryManager::createTriangleMesh() {
	std::string uid = generateUID(MeshType::Triangle);
	return retrieveAdd(uid, [&]() { return std::make_shared<TriangleMesh>(device); });
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