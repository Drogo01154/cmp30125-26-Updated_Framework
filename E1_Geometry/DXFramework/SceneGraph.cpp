#include "SceneGraph.h"

SceneGraph::SceneGraph(ShaderManager* shaderManager, InstanceManager* instanceManager, GeometryManager* geometryManager, MaterialManager* materialManager) : 
	shaderManager(shaderManager), 
	instanceManager(instanceManager), 
	geometryManager(geometryManager),
	materialManager(materialManager)
{
	selectedCreateCamera = -1;
	selectedCreateLight = -1;
	selectedCreateMesh = -1;
	selectedCreateModel = -1;
	meshSelectedMaterial = -1;
	inputResolution = 20;
	ambientLight = XMFLOAT4(0.05f, 0.05f, 0.05f, 1.0f);

	childNameBuffer[0] = '\0'; // first element is null, makes it an empty string
	nodeNameBuffer[0] = '\0'; // first element is null, makes it an empty string

	createBaseScene();
}

void SceneGraph::resetScene(bool loadBasics) {
	materialManager->deleteAllMaterials();
	root.reset();

	size_t test1 = instanceManager->getNumberOfCameras();
	size_t test2 = instanceManager->getNumberOfLights();
	size_t test3 = instanceManager->getNumberOfMeshes();

	if (loadBasics) {
		materialManager->createMaterial("Default");
		createBaseScene();
	}
}

void SceneGraph::SceneGraph::createBaseScene() {
	root = std::make_shared<sceneNode>();
	root->name = "Scene";
	root->m_transform.computeGlobalMatrix();
	auto CameraNode = createChild("Default Camera", root);

	CameraNode->cameraInstance = instanceManager->createFPCamera(CameraNode->cameraID);
	if (CameraNode->cameraInstance.IsValid()) {
		std::shared_ptr<Camera> camera = CameraNode->cameraInstance->camera;
		camera->m_transform.setParent(&CameraNode->m_transform);
		camera->m_transform.setPosition(10.0f, 5.0f, -5.0f);
		camera->update();
		CameraNode->cameraInstance->camera;
	}

	std::shared_ptr<sceneNode> planeNode = createChild("Ground Plane", root);

	planeNode->meshInstance = instanceManager->createPlaneMeshInstance(planeNode->meshID, inputResolution);
	GeometryData* mesh = planeNode->meshInstance.Get();
	mesh->m_transform.setParent(&planeNode->m_transform);
	mesh->m_transform.computeGlobalMatrix(true);

	std::shared_ptr<sceneNode> sphereNode = createChild("Sphere", root);
	
	sphereNode->meshInstance = instanceManager->createSphereMeshInstance(sphereNode->meshID, inputResolution);
	mesh = sphereNode->meshInstance.Get();
	mesh->m_transform.setParent(&sphereNode->m_transform);
	mesh->m_transform.setPosition(XMFLOAT3(10.f, 3.f, 10.f), false);
	mesh->m_transform.setEulerZ(XMConvertToRadians(90.f));
	mesh->m_transform.computeGlobalMatrix(true);
	


	std::shared_ptr<sceneNode> lightNode = createChild("Point Light", root);
	lightNode->lightInstance = instanceManager->createLight(lightNode->lightID, lightTypes::point);
	if (lightNode->lightInstance.IsValid()) {
		Light& light = *lightNode->lightInstance;
		light.m_transform.setParent(&lightNode->m_transform);
		light.m_transform.setPosition(XMFLOAT3(10.0f, 20.f, 10.0f));
		//light.m_transform.setEulerX(XMConvertToRadians(90.f));
		light.updateGlobals(true);
		shaderManager->SetModuleDirtyflags(DirtyModuleFlags::LIGHTNUMCHANGED);
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
	bool isSelected = (selectedNode.lock() == node);

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

void SceneGraph::deleteNode(std::shared_ptr<sceneNode>& node) {

	if (selectedNode.lock() == node) {
		childNameBuffer[0] = '\0'; // first element is null, makes it an empty string
		nodeNameBuffer[0] = '\0'; // first element is null, makes it an empty string
		selectedNode.reset();
	}

	if (std::shared_ptr<sceneNode> parent = node->parent.lock())
	{
		size_t parentChildSize = parent->children.size();
		for (int i = 0; i < parentChildSize; i++) {
			if (parent->children[i] == node) {
				parent->children.erase(parent->children.begin() + i);
				break;
			}
		}
	}
	node.reset();
}

void SceneGraph::selectNode(std::shared_ptr<sceneNode> node) {
	selectedNode = node;
	strncpy_s(nodeNameBuffer, node->name.c_str(), sizeof(nodeNameBuffer));
	nodeNameBuffer[sizeof(nodeNameBuffer) - 1] = '\0'; // ensure null-terminated
	childNameBuffer[0] = '\0'; // first element is null, makes it an empty string

	if (node->meshInstance.IsValid()) {
		const std::string& materialName = node->meshInstance->mat->MaterialName;
		const std::vector<std::string>* materialVec = materialManager->getMaterialStrings();
		meshSelectedMaterial = -1;
		for (int i = 0; i < materialVec->size(); ++i) {
			if (materialVec->at(i) == materialName) {
				meshSelectedMaterial = i;
				break;
			}
		}
	}
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
			if (selectedCreateModel >= 0 && ImGui::Button("Attach Model")) {
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
	if (selectedCreateLight >= 0 && ImGui::Button("Attach Light")) {
		node->lightInstance = instanceManager->createLight(node->lightID, static_cast<lightTypes>(selectedCreateLight));
		if (node->lightInstance.IsValid()) {
			Light& light = *node->lightInstance;
			light.m_transform.setParent(&node->m_transform);
			light.updateGlobals(true);
			shaderManager->SetModuleDirtyflags(DirtyModuleFlags::LIGHTNUMCHANGED);
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
	if (selectedCreateCamera >= 0 && ImGui::Button("Attach Camera")) {
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
			shaderManager->SetModuleDirtyflags(DirtyModuleFlags::CAMERA);
		}
	}

	if (node->lightInstance.IsValid()) {
		node->lightInstance->updateGlobals(false);
		shaderManager->SetModuleDirtyflags(DirtyModuleFlags::LIGHTSDATACHANGED);
	}

	for (auto child : node->children) {
		updateNodeGlobals(child, false);
	}
}

void SceneGraph::nodeImGui(std::shared_ptr<sceneNode> node) {
	ImGui::Text(("Node: " + node->name + ": Details: ").c_str());
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
				if (mesh->m_transform.imGuiRender("Mesh Transform", 1, true, true, true)) {
					mesh->m_transform.computeGlobalMatrix(true);
				}
				const std::vector<const char*>* materialCharVec = materialManager->getMatrialsChars();
				if (ImGui::Combo("Mesh Material: ", &meshSelectedMaterial, materialCharVec->data(), materialCharVec->size())) {
					if (meshSelectedMaterial >= 0) {
						const std::vector<std::string>* materialStringVec = materialManager->getMaterialStrings();
						mesh->mat = materialManager->getMaterialInstance((*materialStringVec)[meshSelectedMaterial]);
					}
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
				if (cameraInstance->camera->imGuiRender(2, cameraInstance->type)) { shaderManager->SetModuleDirtyflags(DirtyModuleFlags::CAMERA); }
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
				bool lightProjectionChanged = false;
				if (light->imGuiRender(3, lightProjectionChanged)) { shaderManager->SetModuleDirtyflags(DirtyModuleFlags::LIGHTSDATACHANGED); }
				if (lightProjectionChanged) { shaderManager->SetModuleDirtyflags(DirtyModuleFlags::LIGHTPROJECTIONCHANGED); }
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
}

void SceneGraph::imGuiRender() {
	if (ImGui::CollapsingHeader("Ambient")) {
		if (ImGui::ColorPicker4("Ambient Colour", &ambientLight.x)) {
			shaderManager->SetModuleDirtyflags(DirtyModuleFlags::CAMERA);
		}
	}
	if (ImGui::CollapsingHeader("SceneGraph")) {
		nodeIncrement = 0;
		graphImGui(root);
	}
	
	if (selectedNode.lock() && ImGui::CollapsingHeader("NodeDetails")) {
		nodeImGui(selectedNode.lock());
	}
}

void SceneGraph::setAmbientLight(const XMFLOAT4& ambientLight) {
	this->ambientLight = ambientLight;
}

const XMFLOAT4& SceneGraph::getAmbientLight() {
	return ambientLight;
}

nlohmann::json SceneGraph::NodeToJson(std::shared_ptr<sceneNode> node) {
	nlohmann::json nodeJson;
	nodeJson["NodeName"] = node->name;
	nodeJson["Transform"] = node->m_transform;

	if (node->cameraInstance.IsValid()) {
		nodeJson["CameraID"] = static_cast<int64_t>(node->cameraID);
	}
	if (node->lightInstance.IsValid()) {
		nodeJson["LightID"] = static_cast<int64_t>(node->lightID);
	}
	if (node->meshInstance.IsValid()) {
		nodeJson["MeshID"] = static_cast<int64_t>(node->meshID);
	}

	if (node->children.size() > 0) {
		 
		 nlohmann::json& childArray = nodeJson["Children"];
		 childArray = nlohmann::json::array();
		 for(auto childNode : node->children) {
			 childArray.push_back(NodeToJson(childNode));
		 }
	}
	return nodeJson;
}

void SceneGraph::to_json(nlohmann::json& j) {

	j["Ambient"] = ambientLight;
	instanceManager->to_json(j["Instances"]);

	j["Root"] = NodeToJson(root);
}

void SceneGraph::from_json(CONST nlohmann::json& j) {

	selectedNode.reset();
	resetScene(false);

	ambientLight = j.at("Ambient").get<XMFLOAT4>();

	std::unordered_map<size_t, size_t> newLightIDMap;
	std::unordered_map<size_t, size_t> newMeshIDMap;
	std::unordered_map<size_t, size_t> newCameraIDMap;

	instanceManager->setDestroyNoInstances(false);

	instanceManager->from_json(j["Instances"], &newCameraIDMap, &newMeshIDMap, &newLightIDMap);

	std::function<std::shared_ptr<sceneNode>(const nlohmann::json&, std::shared_ptr<sceneNode>)> nodeFromJson =
		[&](const nlohmann::json& nodeJson, std::shared_ptr<sceneNode> parent) -> std::shared_ptr<sceneNode> {
		std::shared_ptr<sceneNode> newNode = std::make_shared<sceneNode>();
		newNode->parent = parent;

		newNode->name = nodeJson.at("NodeName").get<std::string>();
		newNode->m_transform = nodeJson.at("Transform").get<Transform>();

		if (parent != nullptr) {
			newNode->m_transform.setParent(&parent->m_transform);
		}
		newNode->m_transform.computeGlobalMatrix(true);

		if (nodeJson.contains("CameraID")) {
			size_t oldID = static_cast<size_t>(nodeJson.at("CameraID").get<uint64_t>());
			newNode->cameraID = newCameraIDMap.at(oldID);
			newNode->cameraInstance = instanceManager->tryGetCameraInstance(newNode->cameraID);
			if (CameraData* camera = newNode->cameraInstance.Get()) {
				camera->camera->m_transform.setParent(&newNode->m_transform);
				camera->camera->updateGlobals(true);
			}
		}

		if (nodeJson.contains("LightID")) {
			size_t oldID = static_cast<size_t>(nodeJson.at("LightID").get<uint64_t>());
			newNode->lightID = newLightIDMap.at(oldID);
			newNode->lightInstance = instanceManager->tryGetLightInstance(newNode->lightID);
			if (Light* light = newNode->lightInstance.Get()) {
				light->m_transform.setParent(&newNode->m_transform);
				light->updateGlobals(true);
			}
		}

		if (nodeJson.contains("MeshID")) {
			size_t oldID = static_cast<size_t>(nodeJson.at("MeshID").get<uint64_t>());
			newNode->meshID = newMeshIDMap.at(oldID);
			newNode->meshInstance = instanceManager->tryGetGeometryInstance(newNode->meshID);
			if (GeometryData* meshInstance = newNode->meshInstance.Get()) {
				meshInstance->m_transform.setParent(&newNode->m_transform);
				meshInstance->m_transform.computeGlobalMatrix(true);
			}
		}

		if (nodeJson.contains("Children")) {
			const nlohmann::json& children = nodeJson["Children"];
			size_t numberOfChildren = children.size();
			newNode->children.resize(numberOfChildren);
			for (int i = 0; i < numberOfChildren; ++i) {
				newNode->children[i] = nodeFromJson(children[i], newNode);
			}
		}

		return newNode;
		};

	root = nodeFromJson(j["Root"], nullptr);

	instanceManager->setDestroyNoInstances(true);
}
