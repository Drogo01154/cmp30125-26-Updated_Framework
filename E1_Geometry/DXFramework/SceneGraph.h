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

	CameraInstance cameraInstance;
	GeometryInstance meshInstance;
	LightInstance lightInstance;

	std::vector<std::shared_ptr<sceneNode>> children;
	std::shared_ptr<sceneNode> parent;
};

class SceneGraph {
public:
	SceneGraph() = delete;
	SceneGraph(ShaderManager* shaderManager, InstanceManager* instanceManager, GeometryManager* geometryManager);


	void createBaseScene();

	std::shared_ptr<sceneNode> createChild(const std::string& name = "Unamed", std::shared_ptr<sceneNode> parent = nullptr);
	void graphImGui(std::shared_ptr<sceneNode> node);

	void deleteNodeMesh(std::shared_ptr<sceneNode> node);

	void deleteNodeCamera(std::shared_ptr<sceneNode> node);

	void deleteNodeLight(std::shared_ptr<sceneNode> node);

	//ImGui function for attaching mesh to scene node
	void imGuiMeshCreation(std::shared_ptr<sceneNode> node);
	void imGuiCameraCreation(std::shared_ptr<sceneNode> node);
	void imGuiLightCreation(std::shared_ptr<sceneNode> node);

	void updateNodeGlobals(std::shared_ptr<sceneNode> node, bool updateLocal);

	void nodeImGui(std::shared_ptr<sceneNode> node);

	void imGuiRender();
	void setAmbientLight(const XMFLOAT4& ambientLight);
	const XMFLOAT4& getAmbientLight();

	void to_json(nlohmann::json& j);

	void from_json(const nlohmann::json& j);

private:
	ShaderManager* shaderManager;
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