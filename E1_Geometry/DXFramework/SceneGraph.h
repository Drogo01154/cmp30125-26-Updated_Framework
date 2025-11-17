#pragma once

#ifndef _SCENEGRAPH_H_
#define _SCENEGRAPH_H_

#include "InstanceManager.h"
#include "FileHandler.h"


struct sceneNode {
	std::string name;
	Transform m_transform;
	std::weak_ptr<Camera> camera;
	size_t meshID;
	size_t lightID;
	size_t cameraID;
	std::weak_ptr<GeometryInstance> meshInstance;
	std::weak_ptr<Light> light;

	std::vector<std::shared_ptr<sceneNode>> children;
	std::vector<std::shared_ptr<sceneNode>> parent;
};

class SceneGraph {

	SceneGraph() = delete;
	SceneGraph(InstanceManager* instanceManager, GeometryManager* geometryManager) : instanceManager(instanceManager), geometryManager(geometryManager)
	{
		selectedCreateCamera = -1;
		selectedCreateLight = -1;
		selectedCreateMesh = -1;
		selectedModel = -1;
		selectedCreateModel = -1;
		inputResolution = 100;
	}

	inline void createChild(const std::string& name = "Unamed", std::shared_ptr<sceneNode> parent) {
		std::shared_ptr<sceneNode> newNode = std::make_shared<sceneNode>();
		newNode->m_transform.setParent(&parent->m_transform);
		newNode->name = name;
		newNode->m_transform.computeGlobalMatrix();
		parent->children.push_back(newNode);
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
		bool node_open = ImGui::TreeNodeEx((node->name +"##"+std::to_string(nodeIncrement)).c_str(), flags);

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
		node->meshInstance.reset();
	}

	inline void deleteNodeCamera(std::shared_ptr<sceneNode> node) {
		instanceManager->removeCameraInstance(node->cameraID);
		node->camera.reset();
	}

	inline void deleteNodeLight(std::shared_ptr<sceneNode> node) {
		instanceManager->removeLightInstance(node->lightID);
		node->light.reset();
	}

	inline void ImGuiMeshCreation(std::shared_ptr<sceneNode> node) {
		bool meshExists = !node->meshInstance.expired();
		ImGui::Text(meshExists ? "Replace Mesh: " : "Add Mesh: ");
		
		if (ImGui::Combo(
			"Mesh Type: ",
			&selectedCreateMesh,
			ThreeDimensionalMeshTypeStrings,
			IM_ARRAYSIZE(ThreeDimensionalMeshTypeStrings)))
		{}
		if (selectedCreateMesh >= 0) {
			MeshType createType = static_cast<MeshType>(selectedCreateMesh);
			
			//If imported Model Type
			if (selectedCreateMesh < 2)
			{
				const std::vector<const char*>* modelList = &FileHandler::get().getModelList();
				if (ImGui::Combo(
					"Imported Model: ",
					&selectedCreateModel,
					modelList->data(),
					modelList->size()

				)) {}
				if (selectedCreateModel > 0 && ImGui::Button("Attach Model")) {
					if (createType == MeshType::AModel) {
						node->meshInstance = instanceManager->createAModelInstance(node->meshID, std::string((*modelList)[selectedCreateModel]));
					}
					else {
						node->meshInstance = instanceManager->createModelInstance(node->meshID, std::string((*modelList)[selectedCreateModel]));
					}
				}
			} else if (selectedCreateMesh < 6) { //If no paramaters
				switch (createType) {
				case MeshType::Point:
					if (ImGui::Button("Attach Point Mesh")) {
						node->meshInstance = instanceManager->createPointMeshInstance(node->meshID);
					}
					break;
				case MeshType::Quad:
					if (ImGui::Button("Attach Quad Mesh")) {
						node->meshInstance = instanceManager->createQuadMeshInstance(node->meshID);
					}
					break;
				case MeshType::Tesselation:
					if (ImGui::Button("Attach Tesselation Mesh")) {
						node->meshInstance = instanceManager->createTesselationMeshInstance(node->meshID);
					}
					break;
				case MeshType::Triangle:
					if (ImGui::Button("Attach Triangle Mesh")) {
						node->meshInstance = instanceManager->createTriangleMeshInstance(node->meshID);
					}
					break;
				}
			} else { // if resolution paramater
				if (ImGui::InputInt("Integer Value", &inputResolution)) {}
				if (inputResolution > 1) {
					switch (createType) {
					case MeshType::Cube:
						if (ImGui::Button("Attach Cube Mesh")) {
							node->meshInstance = instanceManager->createCubeMeshInstance(node->meshID, inputResolution);
						}
						break;
					case MeshType::Plane:
						if (ImGui::Button("Attach Plane Mesh")) {
							node->meshInstance = instanceManager->createPlaneMeshInstance(node->meshID, inputResolution);
						}
						break;
					case MeshType::Sphere:
						if (ImGui::Button("Attach Sphere Mesh")) {
							node->meshInstance = instanceManager->createSphereMeshInstance(node->meshID, inputResolution);
						}
						break;
					}
				}
			}
		}
	}

	inline void ImGuiLightCreation(std::shared_ptr<sceneNode> node) {
		if (ImGui::Combo(
			"Light Type: ",
			&selectedCreateLight,
			Light::GetLightTypeStrings().data(),
			3))
		{
		}
	}

	inline void ImGuiCameraCreation(std::shared_ptr<sceneNode> node) {

	}

	inline void nodeImGui(std::shared_ptr<sceneNode> node) {
		if (ImGui::CollapsingHeader(("Node " + node->name + ": Details:").c_str()))
		{
			node->m_transform.ImGuiRender("Node Transform: ", 0, true, true, true);
			if (!node->meshInstance.expired() && ImGui::CollapsingHeader(("Node " + node->name + " Mesh Settings").c_str())) {
				ImGuiMeshCreation(node);
			}
			if (!node->camera.expired() && ImGui::CollapsingHeader(("Node " + node->name + " Camera Settings").c_str())) {
				//Camera ImGui settings
			}
			if (!node->light.expired() && ImGui::CollapsingHeader(("Node " + node->name + " Light Settings").c_str())) {
				node->light.lock()->ImGuiRender(3);
			}
		}

	}

	inline void ImGuiRender() {
		nodeIncrement = 0;
		graphImGui(root);
		if (selectedNode != nullptr) {
			nodeImGui(selectedNode);
		}
	}


private:
	GeometryManager* geometryManager;
	InstanceManager* instanceManager;
	std::shared_ptr<sceneNode> root;
	std::shared_ptr<sceneNode> selectedNode;
	size_t nodeIncrement;
	size_t selectedModel;
	
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