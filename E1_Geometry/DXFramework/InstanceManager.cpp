#include "InstanceManager.h"

InstanceManager::InstanceManager(GeometryManager* geometryManager, MaterialManager* materialManager, Input* input, HWND hwnd, int screenWidth, int screenHeight) :
	geometryManager(geometryManager), 
	materialManager(materialManager),
	input(input),
	hwnd(hwnd),
	screenWidth(screenWidth),
	screenHeight(screenHeight)
{
}

std::weak_ptr<GeometryInstance> InstanceManager::getGeometryInstance(size_t ID) {
	return geometryInstances.getID(ID);
}
std::weak_ptr<Light> InstanceManager::getLightInstance(size_t ID) {
	return lightInstances.getID(ID);
}
std::weak_ptr<CameraInstance> InstanceManager::getCameraInstance(size_t ID) {
	return cameraInstances.getID(ID);
}

std::string InstanceManager::buildMeshUID(std::weak_ptr<GeometryInstance> instance) {

	if (auto mesh = instance.lock()) { // lock returns shared_ptr
		switch (mesh->type) {
		case MeshType::POINT:
		case MeshType::QUAD:
		case MeshType::TESSELATION:
		case MeshType::TRIANGLE:
			return geometryManager->generateUID(mesh->type);
		case MeshType::AMODEL:
		case MeshType::MODEL:
			return geometryManager->generateUID(mesh->type, std::get<std::string>(mesh->params["modelFile"]));
		case MeshType::CUBE:
		case MeshType::PLANE:
		case MeshType::SPHERE:
			return geometryManager->generateUID(mesh->type, std::get<int>(mesh->params["resolution"]));
		case MeshType::ORTHO:
			return geometryManager->generateUID(mesh->type,
				std::get<int>(mesh->params["Width"]),
				std::get<int>(mesh->params["Height"]),
				std::get<int>(mesh->params["XPosition"]),
				std::get<int>(mesh->params["YPosition"]));
		default:
			throw std::runtime_error("Error: type is not a mesh at: " + static_cast<int>(mesh->type));
		}
	}
	throw std::runtime_error("Error: GeometryInstance has expired");
}

void InstanceManager::removeGeometryInstance(size_t ID) {
	
	if (auto mesh = geometryInstances.getID(ID)) { // lock returns shared_ptr
		//Remove objects material
		mesh->mat = nullptr;
		materialManager->removeMaterialReference(mesh->materialID);

		//Remove objects geometry
		mesh->mesh = nullptr;
		geometryManager->checkRemove(buildMeshUID(mesh));
	}
	geometryInstances.eraseID(ID);
}
void InstanceManager::removeLightInstance(size_t ID) {
	lightInstances.eraseID(ID);
}
void InstanceManager::removeCameraInstance(size_t ID) {
	cameraInstances.eraseID(ID);
}

size_t InstanceManager::getNumberOfLights() { return lightInstances.size(); }
size_t InstanceManager::getNumberOfMeshes() { return geometryInstances.size(); }
size_t InstanceManager::getNumberOfCameras() { return cameraInstances.size(); }

void InstanceManager::clearGeometry() {
	geometryInstances.forEach([&](size_t id, std::shared_ptr<GeometryInstance>& val) {
		val->mat = nullptr;
		val->mesh = nullptr;
		materialManager->removeMaterialReference(val->materialID);
		geometryManager->checkRemove(buildMeshUID(val));
		});
	geometryInstances.clear();
}
void InstanceManager::clearLights() {
	lightInstances.clear();
}
void InstanceManager::clearCameras() {
	cameraInstances.clear();
}
void InstanceManager::clearAll() {
	clearLights();
	clearCameras();
	clearGeometry();
}

std::shared_ptr<GeometryInstance> InstanceManager::createGeometryInstance(size_t& instanceID, MeshType type, std::shared_ptr<BaseMesh> newMesh) {
	auto newInstance = geometryInstances.emplaceID(GeometryInstance());
	newInstance.second->materialID = materialManager->getMaterialID("Default");
	newInstance.second->mat = materialManager->getMaterial(newInstance.second->materialID);
	newInstance.second->type = type;
	newInstance.second->mesh = newMesh;
	instanceID = newInstance.first;
	return newInstance.second;
}

//Create Geometry
std::weak_ptr<GeometryInstance> InstanceManager::createAModelInstance(size_t& instanceID, const std::string& file) {
	std::shared_ptr<GeometryInstance> newInstance = createGeometryInstance(instanceID, MeshType::AMODEL, geometryManager->createAModel(file));
	newInstance->params.emplace("ModelFile", file);
	return newInstance;
}
std::weak_ptr<GeometryInstance> InstanceManager::createCubeMeshInstance(size_t& instanceID, int resolution) {
	std::shared_ptr<GeometryInstance> newInstance = createGeometryInstance(instanceID, MeshType::CUBE, geometryManager->createCubeMesh(resolution));
	newInstance->params.emplace("Resolution", resolution);
	return newInstance;
}
std::weak_ptr<GeometryInstance> InstanceManager::createModelInstance(size_t& instanceID, const std::string& file) {
	std::shared_ptr<GeometryInstance> newInstance = createGeometryInstance(instanceID, MeshType::MODEL, geometryManager->createModel(file));
	newInstance->params.emplace("ModelFile", file);
	return newInstance;
}
std::weak_ptr<GeometryInstance> InstanceManager::createOrthoMeshInstance(size_t& instanceID, int width, int height, int xPosition, int yPosition) {
	std::shared_ptr<GeometryInstance> newInstance = createGeometryInstance(instanceID, MeshType::CUBE, geometryManager->createOrthoMesh(width, height, xPosition, yPosition));
	newInstance->params.emplace("Width", width);
	newInstance->params.emplace("Height", height);
	newInstance->params.emplace("XPosition", xPosition);
	newInstance->params.emplace("YPosition", yPosition);
	return newInstance;
}
std::weak_ptr<GeometryInstance> InstanceManager::createPlaneMeshInstance(size_t& instanceID, int resolution) {
	std::shared_ptr<GeometryInstance> newInstance = createGeometryInstance(instanceID, MeshType::PLANE, geometryManager->createPlaneMesh(resolution));
	newInstance->params.emplace("Resolution", resolution);
	return newInstance;
}
std::weak_ptr<GeometryInstance> InstanceManager::createPointMeshInstance(size_t& instanceID) {
	std::shared_ptr<GeometryInstance> newInstance = createGeometryInstance(instanceID, MeshType::POINT, geometryManager->createPointMesh());
	return newInstance;
}
std::weak_ptr<GeometryInstance> InstanceManager::createQuadMeshInstance(size_t& instanceID) {
	std::shared_ptr<GeometryInstance> newInstance = createGeometryInstance(instanceID, MeshType::QUAD, geometryManager->createQuadMesh());
	return newInstance;
}
std::weak_ptr<GeometryInstance> InstanceManager::createSphereMeshInstance(size_t& instanceID, int resolution) {
	std::shared_ptr<GeometryInstance> newInstance = createGeometryInstance(instanceID, MeshType::SPHERE, geometryManager->createSphereMesh(resolution));
	newInstance->params.emplace("Resolution", resolution);
	return newInstance;
}
std::weak_ptr<GeometryInstance> InstanceManager::createTesselationMeshInstance(size_t& instanceID) {
	std::shared_ptr<GeometryInstance> newInstance = createGeometryInstance(instanceID, MeshType::TESSELATION, geometryManager->createTesselationMesh());
	return newInstance;
}
std::weak_ptr<GeometryInstance> InstanceManager::createTriangleMeshInstance(size_t& instanceID) {
	std::shared_ptr<GeometryInstance> newInstance = createGeometryInstance(instanceID, MeshType::TRIANGLE, geometryManager->createTriangleMesh());
	return newInstance;
}

//Create Lights
std::pair<size_t, std::shared_ptr<Light>>  InstanceManager::createLight(lightTypes type) {
	return lightInstances.emplaceID(Light(type));
}

//Create Cameras
std::pair<size_t, std::shared_ptr<CameraInstance>> InstanceManager::createCamera() {
	auto newInstance = cameraInstances.emplaceID(CameraInstance());
	newInstance.second->type = CameraTypes::BASIC;
	newInstance.second->camera = std::make_shared<Camera>(Camera());
	return newInstance;
}
std::pair<size_t, std::shared_ptr<CameraInstance>> InstanceManager::createFPCamera() {
	auto newInstance = cameraInstances.emplaceID(CameraInstance());
	newInstance.second->type = CameraTypes::BASIC;
	newInstance.second->camera = std::make_shared<Camera>(FPCamera(input, screenWidth, screenHeight, hwnd));
	return newInstance;
}

std::shared_ptr<CameraInstance> InstanceManager::getActiveCamera() {
	return activeCamera;
}
void InstanceManager::setActiveCamera(size_t ID) {
	if (auto camera = cameraInstances.getID(ID))
	{
		activeCamera = camera;
		activeCameraID = ID;
	}
	else {
		throw std::runtime_error("Error: Camera does not exist at ID: " + std::to_string(ID));
	}
}


void InstanceManager::to_json(nlohmann::json& j) {
	//Set j as object
	j = nlohmann::json::object();

	materialManager->to_json(j["Materials"]);

	//Serialize number of instances
	j["CameraNumber"] = cameraInstances.size();
	j["LightNumber"] = lightInstances.size();
	j["MeshNumber"] = geometryInstances.size();

	j["LightInstances"] = nlohmann::json::array();
	j["CameraInstances"] = nlohmann::json::array();
	j["MeshInstances"] = nlohmann::json::array();
	
	nlohmann::json& cameraArray = j["cameraInstances"];
	cameraInstances.forEach([&](size_t id, std::shared_ptr<CameraInstance>& val) {
		nlohmann::json camJson;
		camJson["ID"] = static_cast<uint64_t>(id);
		camJson["Type"] = val->type;
		camJson["Camera"] = nlohmann::json::object();
		
		//Serialize camera data
		nlohmann::json& camJsonObject = camJson["Camera"];
		camJsonObject["Transform"] = val->camera->m_transform;
		camJsonObject["Speed"] = val->camera->getSpeed();
		camJsonObject["LookSpeed"] = val->camera->getLookSpeed();

		cameraArray.push_back(camJson);
		});
	nlohmann::json& meshArray = j["MeshInstances"];
	geometryInstances.forEach([&](size_t id, std::shared_ptr<GeometryInstance>& val) {
		nlohmann::json meshJson;
		meshJson["ID"] = static_cast<uint64_t>(id);
		meshJson["MaterialID"] = static_cast<uint64_t>(val->materialID);
		meshJson["Transform"] = val->m_transform;
		meshJson["MeshType"] = val->type;
		//Serialize Paramaters
		if (!val->params.empty()) {
			meshJson["Parameters"] = nlohmann::json::object();
			for (const auto& [key, value] : val->params) {
				std::visit([&](auto&& v) {
					meshJson["Parameters"][key] = v;
					}, value);
			}
		}
		meshArray.push_back(meshJson);
		});
	nlohmann::json& lightArray = j["LightInstances"];
	lightInstances.forEach([&](size_t id, std::shared_ptr<Light>& val) {
		nlohmann::json lightJson;
		lightJson["ID"] = static_cast<uint64_t>(id);
		lightJson["Light"] = *val;
		lightArray.push_back(lightJson);
		});
}
void InstanceManager::from_json(
	const nlohmann::json& j,
	std::unordered_map<size_t, size_t>* newCameraIDMap,
	std::unordered_map<size_t, size_t>* newMeshIDMap,
	std::unordered_map<size_t, size_t>* newLightIDMap
	) {
	clearAll();

	std::unordered_map<size_t, size_t> newMaterialMap;
	materialManager->from_json(j["Materials"], &newMaterialMap);

	size_t cameraNumber = static_cast<size_t>(j.at("CameraNumber").get<uint64_t>());
	size_t lightNumber = static_cast<size_t>(j.at("LightNumber").get<uint64_t>());
	size_t meshNumber = static_cast<size_t>(j.at("MeshNumber").get<uint64_t>());

	const nlohmann::json& lightArray = j["LightInstances"];

	for (const auto& lightJson : lightArray) {
		Light light = lightJson.at("Light").get<Light>();
		auto newInstance = lightInstances.emplaceID(light);
		size_t oldID = static_cast<size_t>(lightJson.at("ID").get<uint64_t>());
		newLightIDMap->emplace(oldID, newInstance.first);
	}

	const nlohmann::json& cameraArray = j["CameraInstances"];
	for (const auto& camJson : cameraArray) {
		CameraTypes type = camJson.at("Type").get<CameraTypes>();
		auto newInstance = cameraInstances.emplaceID(CameraInstance());
		size_t oldID = static_cast<size_t>(camJson.at("ID").get<uint64_t>());
		newCameraIDMap->emplace(oldID, newInstance.first);
		newInstance.second->type = type;
		
		//Create new camera instance based on type
		if (type == CameraTypes::FPCAMERA) {
			newInstance.second->camera = std::make_shared<Camera>(FPCamera(input, screenWidth, screenHeight, hwnd));
		}
		else {
			newInstance.second->camera = std::make_shared<Camera>(Camera());
		}

		std::shared_ptr<Camera> newCamera = newInstance.second->camera;
		const nlohmann::json& camData = camJson["Camera"];
		newCamera->m_transform = camData.at("Transform").get<Transform>();
		newCamera->setSpeed(camData.at("Speed").get<float>());
		newCamera->setLookSpeed(camData.at("LookSpeed").get<float>());
	}

	const nlohmann::json& meshArray = j["MeshInstances"];
	for (const auto& meshJson : meshArray) {
		MeshType type = meshJson.at("MeshType").get<MeshType>();
		size_t oldID = static_cast<size_t>(meshJson.at("ID").get<uint64_t>());
		auto newInstance = geometryInstances.emplaceID(GeometryInstance());
		newMeshIDMap->emplace(oldID, newInstance.first);
		newInstance.second->materialID = newMaterialMap[static_cast<size_t>(meshJson.at("MaterialID").get<uint64_t>())];
		newInstance.second->mat = materialManager->getMaterial(newInstance.second->materialID);
		newInstance.second->m_transform = meshJson.at("Transform").get<Transform>();

		if (meshJson.contains("Parameters")) {
			const nlohmann::json& parameters = meshJson["Parameters"];
			for (auto it = parameters.begin(); it != parameters.end(); ++it) {
				if (it->is_string()) {
					newInstance.second->params[it.key()] = it->get<std::string>();
				}
				else if (it->is_number_integer()) {
					newInstance.second->params[it.key()] = it->get<int>();
				}
			}
		}

		switch (newInstance.second->type) {
		case MeshType::AMODEL:
			newInstance.second->mesh = geometryManager->createAModel(std::get<std::string>(newInstance.second->params["ModelFile"]));
			break;
		case MeshType::CUBE:
			newInstance.second->mesh = geometryManager->createCubeMesh(std::get<int>(newInstance.second->params["Resolution"]));
			break;

		case MeshType::MODEL:
			newInstance.second->mesh = geometryManager->createModel(std::get<std::string>(newInstance.second->params["ModelFile"]));
			break;

		case MeshType::ORTHO:
			newInstance.second->mesh = geometryManager->createOrthoMesh(std::get<int>(newInstance.second->params["Width"]), std::get<int>(newInstance.second->params["Height"]), std::get<int>(newInstance.second->params["XPosition"]), std::get<int>(newInstance.second->params["YPosition"]));
			break;

		case MeshType::PLANE:
			newInstance.second->mesh = geometryManager->createPlaneMesh(std::get<int>(newInstance.second->params["Resolution"]));
			break;

		case MeshType::POINT:
			newInstance.second->mesh = geometryManager->createPointMesh();
			break;

		case MeshType::QUAD:
			newInstance.second->mesh = geometryManager->createQuadMesh();
			break;
		case MeshType::SPHERE:
			newInstance.second->mesh = geometryManager->createSphereMesh(std::get<int>(newInstance.second->params["Resolution"]));
			break;
		case MeshType::TESSELATION:
			newInstance.second->mesh = geometryManager->createTesselationMesh();
			break;

		case MeshType::TRIANGLE:
			newInstance.second->mesh = geometryManager->createTriangleMesh();
			break;
		}
	}
}

