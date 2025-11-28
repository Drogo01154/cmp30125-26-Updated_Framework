#include "InstanceManager.h"
#include "SerializationHelpers.h"
struct OrthoMeshData {
	int width;
	int height;
	int xPosition;
	int yPosition;
};

InstanceManager::InstanceManager(
	ShaderManager* shaderManager, GeometryManager* geometryManager, 
	MaterialManager* materialManager, Input* input, HWND hwnd, 
	int screenWidth, int screenHeight) :
	shaderManager(shaderManager),
	geometryManager(geometryManager),
	materialManager(materialManager),
	input(input),
	hwnd(hwnd),
	screenWidth(screenWidth),
	screenHeight(screenHeight),
	geometryCache(),
	lightCache(),
	cameraCache()
{
	cameraCache.addTypeEndHandler([&](CameraData* data) {
		if (data->camera == this->getActiveCamera()->camera) {
			this->shaderManager->SetModuleDirtyflag(DirtyModuleFlags::CAMERA);
		}
	});
	lightCache.addTypeEndHandler([&](Light* data) {
		this->shaderManager->SetModuleDirtyflag(DirtyModuleFlags::LIGHTS);
		});
}

GeometryInstance InstanceManager::tryGetGeometryInstance(size_t ID) {
	return geometryCache.tryGetInstance(ID);
}
LightInstance InstanceManager::tryGetLightInstance(size_t ID) {
	return lightCache.tryGetInstance(ID);
}
CameraInstance InstanceManager::tryGetCameraInstance(size_t ID) {
	return cameraCache.tryGetInstance(ID);
}

void InstanceManager::removeGeometryInstance(size_t ID) {
	geometryCache.RemoveID(ID);
}
void InstanceManager::removeLightInstance(size_t ID) {
	lightCache.RemoveID(ID);
}
void InstanceManager::removeCameraInstance(size_t ID) {
	cameraCache.RemoveID(ID);
}

size_t InstanceManager::getNumberOfLights() { return geometryCache.size(); }
size_t InstanceManager::getNumberOfMeshes() { return lightCache.size(); }
size_t InstanceManager::getNumberOfCameras() { return cameraCache.size(); }

void InstanceManager::clearGeometry() {
	geometryCache.clear();
}
void InstanceManager::clearLights() {
	lightCache.clear();
	shaderManager->SetModuleDirtyflag(DirtyModuleFlags::LIGHTS);
}
void InstanceManager::clearCameras() {
	cameraCache.clear();
	shaderManager->SetModuleDirtyflag(DirtyModuleFlags::CAMERA);
}
void InstanceManager::clearAll() {
	geometryCache.clear();
	lightCache.clear();
	cameraCache.clear();
	shaderManager->SetModuleDirtyflag(DirtyModuleFlags::LIGHTS);
	shaderManager->SetModuleDirtyflag(DirtyModuleFlags::CAMERA);
}

GeometryInstance InstanceManager::createGeometryInstance(size_t& instanceID, MeshInstance mesh, bool addMaterial) {
	GeometryData newData;
	if (addMaterial) {
		newData.mat = materialManager->getMaterialInstance("Default");
	} else {
		newData.mat = MaterialInstance();
	}
	newData.mesh = mesh;
	
	GeometryInstance newInst = geometryCache.emplaceID(instanceID, std::move(newData));
	if (!newInst.IsValid()) {
		throw std::runtime_error("Errror: Could not create new geometry instance!");
	}
	return newInst;
}

//Create Geometry
GeometryInstance InstanceManager::createAModelInstance(size_t& instanceID, const std::string& file) {
	return createGeometryInstance(instanceID, geometryManager->createAModel(file));
}
GeometryInstance InstanceManager::createCubeMeshInstance(size_t& instanceID, int resolution) {
	return createGeometryInstance(instanceID, geometryManager->createCubeMesh(resolution));
}
GeometryInstance InstanceManager::createModelInstance(size_t& instanceID, const std::string& file) {
	return createGeometryInstance(instanceID, geometryManager->createModel(file));
}
GeometryInstance InstanceManager::createOrthoMeshInstance(size_t& instanceID, int width, int height, int xPosition, int yPosition) {
	return createGeometryInstance(instanceID, geometryManager->createOrthoMesh(width, height, xPosition, yPosition), false);
}

GeometryInstance InstanceManager::createOrthoMeshInstance(size_t& instanceID, const OrthoMeshData& orthoData) {
	return createGeometryInstance(instanceID, geometryManager->createOrthoMesh(orthoData), false);
}
GeometryInstance InstanceManager::createPlaneMeshInstance(size_t& instanceID, int resolution) {
	return createGeometryInstance(instanceID, geometryManager->createPlaneMesh(resolution));
}
GeometryInstance InstanceManager::createPointMeshInstance(size_t& instanceID) {
	return createGeometryInstance(instanceID, geometryManager->createPointMesh());
}
GeometryInstance InstanceManager::createQuadMeshInstance(size_t& instanceID) {
	return createGeometryInstance(instanceID, geometryManager->createQuadMesh());
}
GeometryInstance InstanceManager::createSphereMeshInstance(size_t& instanceID, int resolution) {
	return createGeometryInstance(instanceID, geometryManager->createSphereMesh(resolution));
}
GeometryInstance InstanceManager::createTesselationMeshInstance(size_t& instanceID) {
	return createGeometryInstance(instanceID, geometryManager->createTesselationMesh());
}
GeometryInstance InstanceManager::createTriangleMeshInstance(size_t& instanceID) {
	return createGeometryInstance(instanceID, geometryManager->createTriangleMesh());
}

//Create Lights
LightInstance  InstanceManager::createLight(size_t& instanceID, lightTypes type) {
	return lightCache.emplaceID(instanceID, Light(type));
}

//Create Cameras
CameraInstance InstanceManager::createCamera(size_t& instanceID) {
	CameraInstance returnInst =  cameraCache.emplaceID(instanceID, CameraData(CameraTypes::BASIC, std::make_shared<Camera>()));
	setActiveCamera(instanceID);
	return returnInst;
}
CameraInstance InstanceManager::createFPCamera(size_t& instanceID) {
	CameraInstance returnInst = cameraCache.emplaceID(instanceID, CameraData(CameraTypes::FPCAMERA, std::make_shared<FPCamera>(input, screenWidth, screenHeight, hwnd)));
	setActiveCamera(instanceID);
	return returnInst;
}

size_t InstanceManager::getActiveCameraID() { return activeCameraID; }

CameraData* InstanceManager::getActiveCamera() {
	return &cameraCache.getValue(activeCameraID);
}
void InstanceManager::setActiveCamera(size_t ID) {
	if (cameraCache.hasID(ID)) {
		activeCameraID = ID;
		shaderManager->SetModuleDirtyflag(DirtyModuleFlags::CAMERA);
	}
	else {
		throw std::runtime_error("Error: Camera does not exist at ID: " + std::to_string(ID));
	}
}

void InstanceManager::setDestroyNoInstances(bool value) {
	geometryCache.setDeleteNoInstances(value);
	geometryCache.setDeleteNoInstances(value);
	lightCache.setDeleteNoInstances(value);
}

void InstanceManager::to_json(nlohmann::json& j) {
	//Set j as object
	j = nlohmann::json::object();

	materialManager->to_json(j["Materials"]);

	nlohmann::json Meshes;

	//Serialize number of instances
	j["CameraNumber"] = cameraCache.size();
	j["LightNumber"] = lightCache.size();
	j["MeshNumber"] = geometryCache.size();

	j["LightInstances"] = nlohmann::json::array();
	j["CameraInstances"] = nlohmann::json::array();
	j["MeshInstances"] = nlohmann::json::array();
	

	nlohmann::json& cameraArray = j["cameraInstances"];
	
	cameraCache.forEach([&](const size_t& ID, CameraData* data) {
		if (data) {
			
			nlohmann::json camJson;
			camJson["ID"] - static_cast<uint64_t>(ID);
			camJson["Type"] = data->type;
			camJson["Camera"] = nlohmann::json::object();

			//Serialize camera data
			nlohmann::json& camJsonObject = camJson["Camera"];
			camJsonObject["Transform"] = data->camera->m_transform;
			camJsonObject["Speed"] = data->camera->getSpeed();
			camJsonObject["LookSpeed"] = data->camera->getLookSpeed();

			cameraArray.push_back(camJson);
		}
		});
	nlohmann::json& meshes = j["Meshes"];				//Json array of used meshes
	nlohmann::json& meshInstances = j["MeshInstances"];		// Json array of mesh instances
	meshInstances = nlohmann::json::array();
	geometryCache.forEach([&](const size_t& ID, GeometryData* data) {
		if (data) {
			nlohmann::json meshInstanceJson;
			meshInstanceJson["ID"] = static_cast<uint64_t>(ID);
			meshInstanceJson["MaterialName"] = data->mat->MaterialName;
			meshInstanceJson["Transform"] = data->m_transform;

			const std::string meshName = geometryManager->determineMeshUID(data->mesh);
			meshInstanceJson["MeshName"] = meshName;
			if (!Meshes.contains(meshName)) {
				nlohmann::json meshJson;
				meshJson["MeshType"] = data->mesh->type;
				if (!data->mesh->params.empty()) {
					meshJson["Parameters"] = nlohmann::json::object();
					for (const auto& [key, value] : data->mesh->params) {
						std::visit([&](auto&& v) {
							meshJson["Parameters"][key] = v;
							}, value);
					}
				}
				meshes[meshName] = meshJson;
			}
			meshInstances.push_back(meshInstanceJson);
		}
	});

	nlohmann::json& lightArray = j["LightInstances"];
	lightCache.forEach([&](const size_t& ID, Light* data) {
		nlohmann::json lightJson;
		lightJson["ID"] = static_cast<uint64_t>(ID);
		lightJson["Light"] = *data;
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

	materialManager->from_json(j["Materials"]);

	size_t cameraNumber = static_cast<size_t>(j.at("CameraNumber").get<uint64_t>());
	size_t lightNumber = static_cast<size_t>(j.at("LightNumber").get<uint64_t>());
	size_t meshNumber = static_cast<size_t>(j.at("MeshNumber").get<uint64_t>());

	const nlohmann::json& lightArray = j["LightInstances"];

	for (const auto& lightJson : lightArray) {
		Light light = lightJson.at("Light").get<Light>();
		size_t oldID = static_cast<size_t>(lightJson.at("ID").get<uint64_t>());
		size_t newID;
		lightCache.emplaceID(newID, std::move(light));
		newLightIDMap->emplace(oldID, newID);	
	}

	const nlohmann::json& cameraArray = j["CameraInstances"];
	for (const auto& camJson : cameraArray) {
		CameraTypes type = camJson.at("Type").get<CameraTypes>();
		std::shared_ptr<Camera> camera;
		if (type == CameraTypes::FPCAMERA) {
			camera = std::make_shared<Camera>(FPCamera(input, screenWidth, screenHeight, hwnd));
		}
		else {
			camera = std::make_shared<Camera>(Camera());
		}

		const nlohmann::json& camData = camJson["Camera"];
		camera->m_transform = camData.at("Transform").get<Transform>();
		camera->setSpeed(camData.at("Speed").get<float>());
		camera->setLookSpeed(camData.at("LookSpeed").get<float>());

		size_t oldID = static_cast<size_t>(camJson.at("ID").get<uint64_t>());
		size_t newID;

		cameraCache.emplaceID(newID, CameraData(type, camera));

		newCameraIDMap->emplace(oldID, newID);
	}


	const nlohmann::json& meshes = j["Meshes"];				//Json array of used meshes
	geometryManager->setDestroyNoInstances(false);
	
	std::unordered_set<std::string> loadedMeshes;

	const nlohmann::json& meshInstances = j["MeshInstances"];		// Json array of mesh instances

	for (const auto& meshInstanceJson : meshInstances) {

		GeometryData data;
		data.m_transform = meshInstanceJson.at("Transform").get<Transform>();
		data.mat = materialManager->getMaterialInstance(meshInstanceJson.at("MaterialName").get<std::string>());
		
		std::string meshName = meshInstanceJson.at("MeshName").get<std::string>();
		if (!loadedMeshes.contains(meshName)) {
			const nlohmann::json& meshJson = meshes[meshName];
			MeshType meshType = meshJson.at("MeshType").get<MeshType>();

			size_t newID;

			switch (meshType) {
			case MeshType::AMODEL:
				data.mesh = geometryManager->createAModel(meshJson.at("file").get<std::string>());
				break;
			case MeshType::CUBE:
				data.mesh = geometryManager->createCubeMesh(meshJson.at("resolution").get<int>());
				break;

			case MeshType::MODEL:
				data.mesh = geometryManager->createModel(meshJson.at("file").get<std::string>());
				break;

			case MeshType::ORTHO:
				data.mesh = geometryManager->createOrthoMesh(
					meshJson.at("width").get<int>(),
					meshJson.at("height").get<int>(),
					meshJson.at("xPos").get<int>(),
					meshJson.at("yPos").get<int>());
				break;

			case MeshType::PLANE:
				data.mesh = geometryManager->createPlaneMesh(meshJson.at("resolution").get<int>());
				break;

			case MeshType::POINT:
				data.mesh = geometryManager->createPointMesh();
				break;

			case MeshType::QUAD:
				data.mesh = geometryManager->createQuadMesh();
				break;
			case MeshType::SPHERE:
				data.mesh = geometryManager->createSphereMesh(meshJson.at("resolution").get<int>());
				break;
			case MeshType::TESSELATION:
				data.mesh = geometryManager->createTesselationMesh();
				break;

			case MeshType::TRIANGLE:
				data.mesh = geometryManager->createTriangleMesh();
				break;
			}
		}
		else {
			data.mesh = geometryManager->tryGetMesh(meshName);
		}
		size_t oldID = static_cast<size_t>(meshInstanceJson.at("ID").get<uint64_t>());
		size_t newID;

		shaderManager->SetModuleDirtyflag(DirtyModuleFlags::LIGHTS);
		shaderManager->SetModuleDirtyflag(DirtyModuleFlags::CAMERA);

		//Add to cache?
		geometryCache.emplaceID(newID, data);


		newMeshIDMap->emplace(oldID, newID);
		

		//Deserialise mesh instances
	}

	geometryManager->setDestroyNoInstances(true);
}

