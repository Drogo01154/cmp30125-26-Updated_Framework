#pragma once

#ifndef _SCENEGRAPH_H_
#define _SCENEGRAPH_H_

#include "InstanceManager.h"
#include "FileHandler.h"
#include <queue>


struct sceneNode {
	std::string name;
	Transform m_transform;

	size_t meshID;
	size_t lightID;
	size_t cameraID;

	
	std::weak_ptr<CameraInstance> camera;
	std::weak_ptr<GeometryInstance> meshInstance;
	std::weak_ptr<Light> light;

	std::vector<std::shared_ptr<sceneNode>> children;
	std::shared_ptr<sceneNode> parent;
};

class SceneGraph {
public:
	SceneGraph() = delete;
	SceneGraph(InstanceManager* instanceManager, GeometryManager* geometryManager) : instanceManager(instanceManager), geometryManager(geometryManager)
	{
		selectedCreateCamera = -1;
		selectedCreateLight = -1;
		selectedCreateMesh = -1;
		selectedModel = -1;
		selectedCreateModel = -1;
		inputResolution = 100;

		createBaseScene();
	}

	inline void createBaseScene() {
		root = std::make_shared<sceneNode>();
		root->name = "Scene";
		root->parent = nullptr;
		root->m_transform.computeGlobalMatrix();
		auto CameraNode = createChild("Default Camera", root);

		auto [ID, Camera] = instanceManager->createFPCamera();
		CameraNode->camera = Camera;
		CameraNode->cameraID = ID;
		if (auto cameraInstance = CameraNode->camera.lock()) {
			cameraInstance->camera->m_transform.setParent(&CameraNode->m_transform);
			cameraInstance->camera->m_transform.setPosition(0.0f, 0.0f, -10.0f);
			cameraInstance->camera->updateGlobals(true);
			cameraInstance->camera->update();

		}
	}

	inline std::shared_ptr<sceneNode> createChild(const std::string& name = "Unamed", std::shared_ptr<sceneNode> parent = nullptr) {
		std::shared_ptr<sceneNode> newNode = std::make_shared<sceneNode>();
		newNode->m_transform.setParent(&parent->m_transform);
		newNode->parent = parent;
		newNode->name = name;
		newNode->m_transform.computeGlobalMatrix();
		parent->children.push_back(newNode);
		return newNode;
	}
	inline void graphImGui(std::shared_ptr<sceneNode> node)
	{
		nodeIncrement++;
		// Check if this node is currently selected
		bool isSelected = (selectedNode == node);

		// Combine flags: selectable + span full width
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;
		if (isSelected) flags |= ImGuiTreeNodeFlags_Selected;

		// Make the tree node itself selectable
		bool node_open = ImGui::TreeNodeEx((node->name + "##" + std::to_string(nodeIncrement)).c_str(), flags);

		// Detect click
		if (ImGui::IsItemClicked()) {
			selectedNode = node;
		}

		// Recurse into children
		if (node_open) {
			for (auto& child : node->children) {
				graphImGui(child);
			}
			ImGui::TreePop();
		}
	}

	inline void deleteNodeMesh(std::shared_ptr<sceneNode> node) {
		instanceManager->removeGeometryInstance(node->meshID);
		node->meshID = SIZE_MAX;
		node->meshInstance.reset();
	}

	inline void deleteNodeCamera(std::shared_ptr<sceneNode> node) {
		instanceManager->removeCameraInstance(node->cameraID);
		node->cameraID = SIZE_MAX;
		node->camera.reset();
	}

	inline void deleteNodeLight(std::shared_ptr<sceneNode> node) {
		instanceManager->removeLightInstance(node->lightID);
		node->lightID = SIZE_MAX;
		node->light.reset();
	}

	//ImGui function for attaching mesh to scene node
	inline void imGuiMeshCreation(std::shared_ptr<sceneNode> node) {
		bool meshExists = !node->meshInstance.expired();
		ImGui::Text(meshExists ? "Replace Mesh: " : "Add Mesh: ");

		if (ImGui::Combo(
			"Mesh Type: ",
			&selectedCreateMesh,
			ThreeDimensionalMeshTypeStrings,
			IM_ARRAYSIZE(ThreeDimensionalMeshTypeStrings)))
		{
		}
		if (selectedCreateMesh >= 0) {
			MeshType createType = static_cast<MeshType>(selectedCreateMesh);

			bool meshCreated = false;

			//If imported Model Type
			if (selectedCreateMesh < 2)
			{
				const std::vector<const char*>* modelList = FileHandler::get().getModelList();
				if (ImGui::Combo(
					"Imported Model: ",
					&selectedCreateModel,
					modelList->data(),
					modelList->size()

				)) {
				}
				if (selectedCreateModel > 0 && ImGui::Button("Attach Model")) {
					if (createType == MeshType::AMODEL) {
						node->meshInstance = instanceManager->createAModelInstance(node->meshID, std::string((*modelList)[selectedCreateModel]));
					}
					else {
						node->meshInstance = instanceManager->createModelInstance(node->meshID, std::string((*modelList)[selectedCreateModel]));
					}
					meshCreated = true;
				}
			}
			else if (selectedCreateMesh < 6) { //If no paramaters
				switch (createType) {
				case MeshType::POINT:
					if (ImGui::Button("Attach Point Mesh")) {
						node->meshInstance = instanceManager->createPointMeshInstance(node->meshID);
						meshCreated = true;
					}
					break;
				case MeshType::QUAD:
					if (ImGui::Button("Attach Quad Mesh")) {
						node->meshInstance = instanceManager->createQuadMeshInstance(node->meshID);
						meshCreated = true;
					}
					break;
				case MeshType::TESSELATION:
					if (ImGui::Button("Attach Tesselation Mesh")) {
						node->meshInstance = instanceManager->createTesselationMeshInstance(node->meshID);
						meshCreated = true;
					}
					break;
				case MeshType::TRIANGLE:
					if (ImGui::Button("Attach Triangle Mesh")) {
						node->meshInstance = instanceManager->createTriangleMeshInstance(node->meshID);
						meshCreated = true;
					}
					break;
				}
			}
			else { // if resolution paramater
				if (ImGui::InputInt("Integer Value", &inputResolution)) {}
				if (inputResolution > 1) {
					switch (createType) {
					case MeshType::CUBE:
						if (ImGui::Button("Attach Cube Mesh")) {
							node->meshInstance = instanceManager->createCubeMeshInstance(node->meshID, inputResolution);
							meshCreated = true;
						}
						break;
					case MeshType::PLANE:
						if (ImGui::Button("Attach Plane Mesh")) {
							node->meshInstance = instanceManager->createPlaneMeshInstance(node->meshID, inputResolution);
							meshCreated = true;
						}
						break;
					case MeshType::SPHERE:
						if (ImGui::Button("Attach Sphere Mesh")) {
							node->meshInstance = instanceManager->createSphereMeshInstance(node->meshID, inputResolution);
							meshCreated = true;
						}
						break;
					}
				}
			}
			if (meshCreated) {
				if (auto mesh = node->meshInstance.lock()) {
					mesh->m_transform.setParent(&node->m_transform);
					mesh->m_transform.computeGlobalMatrix(true);
				}
			}
		}

	}

	inline void imGuiLightCreation(std::shared_ptr<sceneNode> node) {
		if (ImGui::Combo(
			"Light Type: ",
			&selectedCreateLight,
			Light::GetLightTypeStrings().data(),
			3))
		{}
		if (selectedCreateLight > 0 && ImGui::Button("Attach Light")) {
			auto [ID, Light] = instanceManager->createLight(static_cast<lightTypes>(selectedCreateLight));
			Light->m_transform.setParent(&node->m_transform);
			Light->updateGlobals(true);
			node->light = Light;
			node->lightID = ID;
		}
	}

	inline void imGuiCameraCreation(std::shared_ptr<sceneNode> node) {
		if (ImGui::Combo(
			"Camera Type: ",
			&selectedCreateCamera,
			Camera::GetCameraTypeStrings().data(),
			2
		)) {}
		if (selectedCreateCamera > 0 && ImGui::Button("Attach Camera")) {

			std::function<std::pair<size_t, std::shared_ptr<CameraInstance>>()> createFunction;
			switch (static_cast<CameraTypes>(selectedCreateCamera)) 
			{	
			case CameraTypes::BASIC:
				createFunction = [&]() -> std::pair<size_t, std::shared_ptr<CameraInstance>> {
					instanceManager->createCamera();
					};
				break;
			case CameraTypes::FPCAMERA:
				createFunction = [&]() -> std::pair<size_t, std::shared_ptr<CameraInstance>> {
					instanceManager->createFPCamera();
					};
				break;
			}
			auto [ID, Camera] = createFunction();
			node->camera = Camera;
			node->cameraID = ID;
			if (auto camera = node->camera.lock()) {
				camera->camera->m_transform.setParent(&node->m_transform);
				camera->camera->updateGlobals(true);
			}
		}
	}

	inline void updateNodeGlobals(std::shared_ptr<sceneNode> node, bool updateLocal) {
		
		node->m_transform.computeGlobalMatrix(updateLocal);
		if (auto mesh = node->meshInstance.lock()) {
			mesh->m_transform.computeGlobalMatrix(false);
		}

		if (auto camera = node->camera.lock()) {
			camera->camera->updateGlobals(false);
		}

		if (auto light = node->light.lock()) {
			light->updateGlobals(false);
		}

		for (auto child : node->children) {
			updateNodeGlobals(child, false);
		}
	}

	inline void nodeImGui(std::shared_ptr<sceneNode> node) {
		if (ImGui::CollapsingHeader(("Node " + node->name + ": Details:").c_str()))
		{
			if (node->m_transform.imGuiRender("Node Transform: ", 0, true, true, true)) {
				updateNodeGlobals(node, true);
			}
			if (!node->meshInstance.expired() && ImGui::CollapsingHeader(("Node " + node->name + " Mesh Settings").c_str())) {
				if (auto mesh = node->meshInstance.lock()) {
					if (mesh->m_transform.imGuiRender("Mesh Transform", 1, true, true, false)) {
						mesh->m_transform.computeGlobalMatrix(true);
					}
				}
			} else {
				imGuiMeshCreation(node);
			}
			if (!node->camera.expired() && ImGui::CollapsingHeader(("Node " + node->name + " Camera Settings").c_str())) {
				if (auto cameraInstance = node->camera.lock()) {
					cameraInstance->camera->imGuiRender(2, cameraInstance->type);
				}
			}
			else {
				imGuiCameraCreation(node);
			}
			if (!node->light.expired() && ImGui::CollapsingHeader(("Node " + node->name + " Light Settings").c_str())) {
				if (auto light = node->light.lock()) {
					light->imGuiRender(3);
				}
				node->light.lock()->imGuiRender(3);
			}
			else {
				imGuiLightCreation(node);
			}
		}

	}

	inline void imGuiRender() {
		nodeIncrement = 0;
		graphImGui(root);
		if (selectedNode != nullptr) {
			nodeImGui(selectedNode);
		}
	}

	inline void setAmbientLight(const XMFLOAT4& ambientLight) {
		this->ambientLight = ambientLight;
	}

	inline const XMFLOAT4& getAmbientLight() {
		return ambientLight;
	}

	inline void to_json(nlohmann::json& j) {

		instanceManager->to_json(j["Instances"]);

		std::queue<std::pair <nlohmann::json*, std::shared_ptr<sceneNode>>> nodeQueue;
		nodeQueue.push(std::make_pair(&j["Nodes"], root));
		while(nodeQueue.size() > 0) {
			auto [nodeJsonPtr, nodePtr] = nodeQueue.front();
			nodeQueue.pop();
			(*nodeJsonPtr) = nlohmann::json::object();

			(*nodeJsonPtr)["NodeName"] = nodePtr->name;
			(*nodeJsonPtr)["Transform"] = nodePtr->m_transform;

			if (!nodePtr->camera.expired()) {
				(*nodeJsonPtr)["CameraID"] = static_cast<int64_t>(nodePtr->cameraID);
			}
			if (!nodePtr->light.expired()) {
				(*nodeJsonPtr)["LightID"] = static_cast<int64_t>(nodePtr->lightID);
			}
			if (!nodePtr->meshInstance.expired()) {
				(*nodeJsonPtr)["MeshID"] = static_cast<int64_t>(nodePtr->meshID);
			}

			if (nodePtr->children.size() > 0) {
				auto& childrenJson = (*nodeJsonPtr)["Children"];
				childrenJson = nlohmann::json::array();
				for (auto child : nodePtr->children) {
					childrenJson.push_back(nlohmann::json());
					nodeQueue.push(std::make_pair(&childrenJson.back(), child));
				}
			}
		}
	}

	inline void from_json(const nlohmann::json& j) {
		std::unordered_map<size_t, size_t> newCameraIDMap;
		std::unordered_map<size_t, size_t> newLightIDMap;
		std::unordered_map<size_t, size_t> newMeshIDMap;
		instanceManager->from_json(j, &newCameraIDMap, &newMeshIDMap, &newLightIDMap);

		struct nodeData {
			nodeData(const nlohmann::json* nodeJson,
			std::shared_ptr<sceneNode> node,
			std::shared_ptr<sceneNode> parent) :
				nodeJson(nodeJson),
				node(node),
				parent(parent) { }
			const nlohmann::json* nodeJson;
			std::shared_ptr<sceneNode> node;
			std::shared_ptr<sceneNode> parent;
		};

		std::queue<nodeData> nodeQueue;
		root = std::make_shared<sceneNode>();
		nodeQueue.push(nodeData(&j["Nodes"], root, nullptr));

		while (nodeQueue.size() > 0) {
			nodeData data = nodeQueue.front();
			nodeQueue.pop();
			data.node->name = data.nodeJson->at("NodeName").get<std::string>();

			data.node->parent = data.parent;

			data.node->m_transform = data.nodeJson->at("Transform").get<Transform>();
			if (data.parent != nullptr) {
				data.node->m_transform.setParent(&data.parent->m_transform);
			}
			data.node->m_transform.computeGlobalMatrix(true);

			if (data.nodeJson->contains("CameraID")) {
				data.node->cameraID = newCameraIDMap[static_cast<size_t>(data.nodeJson->at("CameraID").get<uint64_t>())];
				data.node->camera = instanceManager->getCameraInstance(data.node->cameraID);
				if (auto camera = data.node->camera.lock()) {
					camera->camera->m_transform.setParent(&data.node->m_transform);
					camera->camera->updateGlobals(true);
				}
			}

			if (data.nodeJson->contains("LightID")) {
				data.node->lightID = newLightIDMap[static_cast<size_t>(data.nodeJson->at("LightID").get<uint64_t>())];
				data.node->light = instanceManager->getLightInstance(data.node->lightID);
				if (auto light = data.node->light.lock()) {
					light->m_transform.setParent(&data.node->m_transform);
					light->updateGlobals(true);
				}
			}

			if (data.nodeJson->contains("MeshID")) {
				data.node->meshID = newMeshIDMap[static_cast<size_t>(data.nodeJson->at("MeshID").get<uint64_t>())];
				data.node->meshInstance = instanceManager->getGeometryInstance(data.node->meshID);
				if (auto mesh = data.node->meshInstance.lock()) {
					mesh->m_transform.setParent(&data.node->m_transform);
					mesh->m_transform.computeGlobalMatrix(true);
				}
			}
			
			
			if (data.nodeJson->contains("Children"))
			{
				for (auto& jsonChild : (*data.nodeJson)["Children"]) {
					data.node->children.push_back(std::make_shared<sceneNode>());
					nodeQueue.push(nodeData(&jsonChild, data.node->children.back(), data.node));
				}
			}
		}
	}

private:
	GeometryManager* geometryManager;
	InstanceManager* instanceManager;
	std::shared_ptr<sceneNode> root;
	std::shared_ptr<sceneNode> selectedNode;
	size_t nodeIncrement;
	size_t selectedModel;

	XMFLOAT4 ambientLight;

	inline static const char* ThreeDimensionalMeshTypeStrings[10] = {
		"AModel",
		"Model",
		"Point",
		"Quad",
		"Tesselation",
		"Triangle",
		"Cube",
		"Plane",
		"Sphere",
	};
	
	int selectedCreateCamera;
	int selectedCreateLight;
	int selectedCreateMesh;
	int selectedCreateModel;
	int inputResolution;
};

#endif