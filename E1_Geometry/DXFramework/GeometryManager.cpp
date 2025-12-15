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

//Constructor
GeometryManager::GeometryManager(ID3D11Device* device, ID3D11DeviceContext* deviceContext) : meshCache(), device(device), deviceContext(deviceContext) 
{
	//Make sure mesh deleted on zero instances
	meshCache.addTypeEndHandler([](MeshData* data) {
		data->mesh.reset();	// Make pointer empty
		});
}

//Try to retrieve mesh from cache
MeshInstance GeometryManager::tryGetMesh(const std::string& uid) {
	return meshCache.tryGetInstance(uid);
}

//Calculate unique ID for mesh based on its type and input paramaters
std::string GeometryManager::generateUID(MeshType type,
	std::optional<std::string> file,
	std::optional<int> resolution,
	std::optional<int> width,
	std::optional<int> height,
	std::optional<int> xPos,
	std::optional<int> yPos)
{
	std::string uid = meshTypeStrings[static_cast<int>(type)];

	switch (type) {
	case MeshType::POINT:
	case MeshType::QUAD:
	case MeshType::TESSELATION:
	case MeshType::TRIANGLE:
		return uid;

	case MeshType::MODEL:
	case MeshType::AMODEL:
		if (!file) throw std::runtime_error("Model/AModel requires a file name");
		return uid + ": " + *file;

	case MeshType::CUBE:
	case MeshType::PLANE:
	case MeshType::SPHERE:
		if (!resolution) throw std::runtime_error("Cube/Plane/Sphere requires resolution");
		return uid + ": " + std::to_string(*resolution);

	case MeshType::ORTHO:
		if (!width || !height || !xPos || !yPos)
			throw std::runtime_error("Ortho requires width, height, xPos, yPos");
		return uid + ": " + std::to_string(*width) + "_" + std::to_string(*height)
			+ "_" + std::to_string(*xPos) + "_" + std::to_string(*yPos);

	default:
		throw std::runtime_error("Unknown mesh type");
	}
}

//Gets mesh ID byased on data
std::string GeometryManager::determineMeshUID(MeshData* data) {
	if (!data) return "";
	switch (data->type) {
	case MeshType::POINT:
	case MeshType::QUAD:
	case MeshType::TESSELATION:
	case MeshType::TRIANGLE:
		return generateUID(data->type);
	case MeshType::AMODEL:
	case MeshType::MODEL:
		return generateUID(data->type, std::get<std::string>(data->params["file"]));
	case MeshType::CUBE:
	case MeshType::PLANE:
	case MeshType::SPHERE:
		return generateUID(data->type, std::nullopt, std::get<int>(data->params["resolution"]));
	case MeshType::ORTHO:
		return generateUID(data->type,
			std::nullopt,
			std::nullopt,
			std::get<int>(data->params["width"]),
			std::get<int>(data->params["height"]),
			std::get<int>(data->params["xPos"]),
			std::get<int>(data->params["yPos"]));
	default:
		throw std::runtime_error("Error: type is not a mesh at: " + static_cast<int>(data->type));
	}
}

//Retieves existing mesh in cache if exists, otherwise creates and sends it.
MeshInstance GeometryManager::RetrieveAddMesh(MeshType type,
	std::optional<std::string> file,
	std::optional<int> resolution,
	std::optional<int> width,
	std::optional<int> height,
	std::optional<int> xPos,
	std::optional<int> yPos) 
{
	std::string uid = generateUID(type, file, resolution, width, height, xPos, yPos);
	MeshInstance retrieval = tryGetMesh(uid);
	if (!retrieval.IsValid()) {
		std::unordered_map<std::string, ParamValue> params;

		if (file)       params["file"] = *file;
		if (resolution) params["resolution"] = *resolution;
		if (width)      params["width"] = *width;
		if (height)     params["height"] = *height;
		if (xPos)       params["xPos"] = *xPos;
		if (yPos)       params["yPos"] = *yPos;


		std::shared_ptr<BaseMesh> mesh = nullptr;
		switch (type) {
		case MeshType::POINT:		mesh = std::make_shared<PointMesh>(device, deviceContext); break;
		case MeshType::QUAD:		mesh = std::make_shared<QuadMesh>(device, deviceContext); break;
		case MeshType::TESSELATION:	mesh = std::make_shared<TessellationMesh>(device, deviceContext); break;
		case MeshType::TRIANGLE:	mesh = std::make_shared<TriangleMesh>(device, deviceContext); break;

		case MeshType::MODEL:
		case MeshType::AMODEL:
		{
			const std::string& filePath = FileHandler::get().locateModel(*file);

			mesh = (type == MeshType::MODEL)
				? std::dynamic_pointer_cast<BaseMesh>(std::make_shared<Model>(device, deviceContext, filePath.c_str()))
				: std::dynamic_pointer_cast<BaseMesh>(std::make_shared<AModel>(device, filePath));
		}
			break;
		case MeshType::CUBE:		mesh = std::make_shared<CubeMesh>(device, deviceContext, *resolution); break;
		case MeshType::PLANE:		mesh = std::make_shared<PlaneMesh>(device, deviceContext, *resolution);	break;
		case MeshType::SPHERE:		mesh = std::make_shared<SphereMesh>(device, deviceContext, *resolution); break;
		case MeshType::ORTHO:		mesh = std::make_shared<OrthoMesh>(device, deviceContext, *width, *height, *xPos, *yPos); break;
		default:
			throw std::runtime_error("Unknown mesh type");
		}
		
		retrieval = meshCache.emplaceID(uid, std::move(MeshData(type, mesh, params)));
		if (!retrieval.IsValid()) {
			throw std::runtime_error("Error: Could not add mesh: " + uid);
		}
	}
	return retrieval;

}


MeshInstance GeometryManager::createAModel(const std::string& file) {
	return RetrieveAddMesh(MeshType::AMODEL, file);
}

MeshInstance GeometryManager::createCubeMesh(int resolution) {
	return RetrieveAddMesh(MeshType::CUBE, std::nullopt, resolution);
}

MeshInstance GeometryManager::createModel(const std::string& file) {
	return RetrieveAddMesh(MeshType::MODEL, file);
}

MeshInstance GeometryManager::createOrthoMesh(int width, int height, int xPosition, int yPosition) {
	return RetrieveAddMesh(MeshType::ORTHO, std::nullopt, std::nullopt, width, height, xPosition, yPosition);
}

MeshInstance GeometryManager::createOrthoMesh(const OrthoMeshData orthoData) {
	return RetrieveAddMesh(MeshType::ORTHO, std::nullopt, std::nullopt, orthoData.width, orthoData.height, orthoData.xPosition, orthoData.yPosition);
}

MeshInstance GeometryManager::createPlaneMesh(int resolution) {
	return RetrieveAddMesh(MeshType::PLANE, std::nullopt, resolution);
}

MeshInstance GeometryManager::createPointMesh() {
	return RetrieveAddMesh(MeshType::POINT);
}

MeshInstance GeometryManager::createQuadMesh() {
	return RetrieveAddMesh(MeshType::QUAD);
}

MeshInstance GeometryManager::createSphereMesh(int resolution) {
	return RetrieveAddMesh(MeshType::SPHERE, std::nullopt, resolution);
}

MeshInstance GeometryManager::createTesselationMesh() {
	return RetrieveAddMesh(MeshType::TESSELATION);
}

MeshInstance GeometryManager::createTriangleMesh() {
	return RetrieveAddMesh(MeshType::TRIANGLE);
}

std::span<const char* const> GeometryManager::GetMeshTypeStrings() {
	return meshTypeStrings;
}

//Makes it so cache does not destroy meshes on no instance active
void GeometryManager::setDestroyNoInstances(bool value) {
	meshCache.setDeleteNoInstances(value);
}