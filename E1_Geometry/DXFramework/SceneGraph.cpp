#include "SceneGraph.h"

SceneGraph::SceneGraph(ShaderManager* shaderManager, InstanceManager* instanceManager, GeometryManager* geometryManager) : 
	shaderManager(shaderManager), 
	instanceManager(instanceManager), 
	geometryManager(geometryManager)
{
	selectedCreateCamera = -1;
	selectedCreateLight = -1;
	selectedCreateMesh = -1;
	selectedModel = -1;
	selectedCreateModel = -1;
	inputResolution = 100;
	ambientLight = { 0.0f, 0.0f, 0.0f, 1.f };

	createBaseScene();
}

void SceneGraph::SceneGraph::createBaseScene() {
	root = std::make_shared<sceneNode>();
	root->name = "Scene";
	root->parent = nullptr;
	root->m_transform.computeGlobalMatrix();
	auto CameraNode = createChild("Default Camera", root);

	CameraNode->cameraInstance = instanceManager->createFPCamera(CameraNode->cameraID);
	if (CameraNode->cameraInstance.IsValid()) {
		std::shared_ptr<Camera> camera = CameraNode->cameraInstance->camera;
		camera->m_transform.setParent(&CameraNode->m_transform);
		camera->m_transform.setPosition(0.0f, 0.0f, -10.0f);
		camera->update();
		CameraNode->cameraInstance->camera;
	}
}

std::shared_ptr<sceneNode> SceneGraph::createChild(const std::string& name, std::shared_ptr<sceneNode> parent) {
	std::shared_ptr<sceneNode> newNode = std::make_shared<sceneNode>();
	newNode->m_transform.setParent(&parent->m_transform);
	newNode->parent = parent;
	newNode->name = name;
	newNode->m_transform.computeGlobalMatrix();
	parent->children.push_back(newNode);
	return newNode;
}
void SceneGraph::graphImGui(std::shared_ptr<sceneNode> node)
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
		selectNode(node);
	}

	// Recurse into children
	if (node_open) {
		for (auto& child : node->children) {
			graphImGui(child);
		}
		ImGui::TreePop();
	}
}

void SceneGraph::deleteNodeMesh(std::shared_ptr<sceneNode> node) {
	node->meshInstance = GeometryInstance();
	node->meshID = SIZE_MAX;
}

void SceneGraph::deleteNodeCamera(std::shared_ptr<sceneNode> node) {
	node->cameraInstance = CameraInstance();
	node->cameraID = SIZE_MAX;
}

void SceneGraph::deleteNodeLight(std::shared_ptr<sceneNode> node) {
	node->lightInstance = LightInstance();
	node->lightID = SIZE_MAX;
}

void SceneGraph::deleteNode(std::shared_ptr<sceneNode> node) {
	if (selectedNode == node) {
		childNameBuffer[0] = '\0'; // first element is null, makes it an empty string
		nodeNameBuffer[0] = '\0'; // first element is null, makes it an empty string
		selectedNode = nullptr;
	}

	if (node->parent) {
		std::vector<std::shared_ptr<sceneNode>>* parentsChildren = &node->parent->children;
		node->parent->children.erase(
			std::remove(node->parent->children.begin(), node->parent->children.end(), node),
			node->parent->children.end()
		);
		while (node->children.size() > 0) {
			deleteNode(*node->children.begin());
		}
		if (node->meshInstance.IsValid()) { deleteNodeMesh(node); }
		if (node->lightInstance.IsValid()) { deleteNodeLight(node); }
		if (node->cameraInstance.IsValid()) { deleteNodeCamera(node); }
	}
}



void SceneGraph::selectNode(std::shared_ptr<sceneNode> node) {
	selectedNode = node;
	strncpy_s(nodeNameBuffer, node->name.c_str(), sizeof(nodeNameBuffer));
	nodeNameBuffer[sizeof(nodeNameBuffer) - 1] = '\0'; // ensure null-terminated
	childNameBuffer[0] = '\0'; // first element is null, makes it an empty string
}

//ImGui function for attaching mesh to scene node
void SceneGraph::imGuiMeshCreation(std::shared_ptr<sceneNode> node) {
	bool meshExists = node->meshInstance.IsValid();
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
			if (node->meshInstance.IsValid()) {
				GeometryData* mesh = node->meshInstance.Get();
				mesh->m_transform.setParent(&node->m_transform);
				mesh->m_transform.computeGlobalMatrix(true);
			}
		}
	}

}

void SceneGraph::imGuiLightCreation(std::shared_ptr<sceneNode> node) {
	if (ImGui::Combo(
		"Light Type: ",
		&selectedCreateLight,
		Light::GetLightTypeStrings().data(),
		3))
	{
	}
	if (selectedCreateLight > 0 && ImGui::Button("Attach Light")) {
		node->lightInstance = instanceManager->createLight(node->lightID, static_cast<lightTypes>(selectedCreateLight));
		if (node->lightInstance.IsValid()) {
			Light& light = *node->lightInstance;
			light.m_transform.setParent(&node->m_transform);
			light.updateGlobals(true);
		}
	}
}

void SceneGraph::imGuiCameraCreation(std::shared_ptr<sceneNode> node) {
	if (ImGui::Combo(
		"Camera Type: ",
		&selectedCreateCamera,
		Camera::GetCameraTypeStrings().data(),
		2
	)) {
	}
	if (selectedCreateCamera > 0 && ImGui::Button("Attach Camera")) {
		switch (static_cast<CameraTypes>(selectedCreateCamera))
		{
		case CameraTypes::BASIC:
			node->cameraInstance = instanceManager->createCamera(node->cameraID);
			break;
		case CameraTypes::FPCAMERA:
			node->cameraInstance = instanceManager->createFPCamera(node->cameraID);
			break;
		}
		if (node->cameraInstance.IsValid()) {
			CameraData* CamData = node->cameraInstance.Get();
			CamData->camera->m_transform.setParent(&node->m_transform);
			CamData->camera->updateGlobals(true);
		}
	}
}

void SceneGraph::updateNodeGlobals(std::shared_ptr<sceneNode> node, bool updateLocal) {

	node->m_transform.computeGlobalMatrix(updateLocal);
	if (node->meshInstance.IsValid()) {
		node->meshInstance->m_transform.computeGlobalMatrix(false);
	}

	if (node->cameraInstance.IsValid()) {
		node->cameraInstance->camera->updateGlobals(false);
		if (instanceManager->getActiveCameraID() == node->cameraID) {
			shaderManager->SetModuleDirtyflag(DirtyModuleFlags::CAMERA);
		}
	}

	if (node->lightInstance.IsValid()) {
		node->lightInstance->updateGlobals(false);
		shaderManager->SetModuleDirtyflag(DirtyModuleFlags::LIGHTS);
	}

	for (auto child : node->children) {
		updateNodeGlobals(child, false);
	}
}

void SceneGraph::nodeImGui(std::shared_ptr<sceneNode> node) {
	if (ImGui::TreeNode(("Node: " + node->name + ": Details:").c_str()))
	{
		if (ImGui::InputText("Node Name", nodeNameBuffer, sizeof(nodeNameBuffer))) {
			node->name = nodeNameBuffer; // update the node name if user types
		}

		bool hasLight = node->lightInstance.IsValid();
		bool hasCamera = node->cameraInstance.IsValid();
		bool hasMesh = node->meshInstance.IsValid();

		//Nodes Transform Settings
		if (ImGui::TreeNode("Transform Settings: ")) {
			if (node->m_transform.imGuiRender("Node Transform: ", 0, true, true, true)) {
				updateNodeGlobals(node, true);
			}
			ImGui::TreePop();
		}

		//Mesh manipulation
		if (hasMesh) {
			if (ImGui::TreeNode(("Node: " + node->name + " Mesh Settings:").c_str())) {
				if (GeometryData* mesh = node->meshInstance.Get()) {
					if (mesh->m_transform.imGuiRender("Mesh Transform", 1, true, true, false)) {
						mesh->m_transform.computeGlobalMatrix(true);
					}
				}
				ImGui::TreePop();
			}
			if (ImGui::Button("Delete Mesh")) { deleteNodeMesh(node); }

		}
		else {
			if (ImGui::TreeNode("Attach Mesh")) {
				imGuiMeshCreation(node);
				ImGui::TreePop();
			}
		}

		//Camera manipulation
		if (hasCamera)
		{
			if (ImGui::TreeNode(("Node: " + node->name + " Camera Settings").c_str())) {
				if (CameraData* cameraInstance = node->cameraInstance.Get()) {
					if (cameraInstance->camera->imGuiRender(2, cameraInstance->type)) { shaderManager->SetModuleDirtyflag(DirtyModuleFlags::CAMERA); }
				}
				ImGui::TreePop();
			}
			if (ImGui::Button("Delete Camera")) { deleteNodeCamera(node); }
		}
		else {
			if (ImGui::TreeNode("Attach Camera")) {
				imGuiCameraCreation(node);
				ImGui::TreePop();
			}
		}
		//Light manipulation
		if (hasLight) {
			if (ImGui::TreeNode(("Node: " + node->name + " Light Settings").c_str())) {
				if (Light* light = node->lightInstance.Get()) {
					if (light->imGuiRender(3)) { shaderManager->SetModuleDirtyflag(DirtyModuleFlags::LIGHTS); }
				}
				ImGui::TreePop();
			}
			if (ImGui::Button("Delete Light")) { deleteNodeLight(node); }
		}
		else {
			if (ImGui::TreeNode("Attach Light")) {
				imGuiLightCreation(node);
				ImGui::TreePop();
			}
		}
		if (ImGui::InputText("New Child Name", childNameBuffer, sizeof(childNameBuffer))) {
		}
		if (ImGui::Button("AttachChild")) {
			createChild(childNameBuffer, node);
		}
		if (ImGui::Button("Delete Node")) {
			deleteNode(node);
		}
		ImGui::TreePop();
	}
}

void SceneGraph::imGuiRender() {
	nodeIncrement = 0;
	if (ImGui::CollapsingHeader("SceneGraph")) {
		graphImGui(root);
	}
	
	if (selectedNode != nullptr && ImGui::CollapsingHeader("NodeDetails")) {
		nodeImGui(selectedNode);
	}
}

void SceneGraph::setAmbientLight(const XMFLOAT4& ambientLight) {
	this->ambientLight = ambientLight;
}

const XMFLOAT4& SceneGraph::getAmbientLight() {
	return ambientLight;
}

void SceneGraph::to_json(nlohmann::json& j) {

	instanceManager->to_json(j["Instances"]);

	std::queue<std::pair <nlohmann::json*, std::shared_ptr<sceneNode>>> nodeQueue;
	nodeQueue.push(std::make_pair(&j["Nodes"], root));
	while (nodeQueue.size() > 0) {
		auto [nodeJsonPtr, nodePtr] = nodeQueue.front();
		nodeQueue.pop();
		(*nodeJsonPtr) = nlohmann::json::object();

		(*nodeJsonPtr)["NodeName"] = nodePtr->name;
		(*nodeJsonPtr)["Transform"] = nodePtr->m_transform;

		if (!nodePtr->cameraInstance.IsValid()) {
			(*nodeJsonPtr)["CameraID"] = static_cast<int64_t>(nodePtr->cameraID);
		}
		if (!nodePtr->lightInstance.IsValid()) {
			(*nodeJsonPtr)["LightID"] = static_cast<int64_t>(nodePtr->lightID);
		}
		if (!nodePtr->meshInstance.IsValid()) {
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
void SceneGraph::from_json(const nlohmann::json& j) {
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
			parent(parent) {
		}
		const nlohmann::json* nodeJson;
		std::shared_ptr<sceneNode> node;
		std::shared_ptr<sceneNode> parent;
	};

	instanceManager->setDestroyNoInstances(false);

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
			data.node->cameraInstance = instanceManager->tryGetCameraInstance(data.node->cameraID);
			if (CameraData* camera = data.node->cameraInstance.Get()) {
				camera->camera->m_transform.setParent(&data.node->m_transform);
				camera->camera->updateGlobals(true);
			}
		}

		if (data.nodeJson->contains("LightID")) {
			data.node->lightID = newLightIDMap[static_cast<size_t>(data.nodeJson->at("LightID").get<uint64_t>())];
			data.node->lightInstance = instanceManager->tryGetLightInstance(data.node->lightID);
			if (Light* light = data.node->lightInstance.Get()) {
				light->m_transform.setParent(&data.node->m_transform);
				light->updateGlobals(true);
			}
		}

		if (data.nodeJson->contains("MeshID")) {
			data.node->meshID = newMeshIDMap[static_cast<size_t>(data.nodeJson->at("MeshID").get<uint64_t>())];
			data.node->meshInstance = instanceManager->tryGetGeometryInstance(data.node->meshID);
			if (GeometryData* meshInstance = data.node->meshInstance.Get()) {
				meshInstance->m_transform.setParent(&data.node->m_transform);
				meshInstance->m_transform.computeGlobalMatrix(true);
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

	instanceManager->setDestroyNoInstances(true);
}

