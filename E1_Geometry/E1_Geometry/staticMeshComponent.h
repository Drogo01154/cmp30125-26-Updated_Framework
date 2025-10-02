#include "entt/entt.hpp"
#include "../DXFramework/BaseShader.h"
#include "../DXFramework/DXF.h"
#include <memory>

enum meshTypes {
	AModel,
	CubeMesh,
	Model,
	OrthoMesh,
	PlaneMesh,
	PointMesh,
	QuadMesh,
	SphereMesh,
	TessellationMesh
};

struct StaticMeshComponent {

	StaticMeshComponent(meshTypes _type) : meshType(_type) {
		if ((_type == meshTypes::Model) || (_type == meshTypes::OrthoMesh) || (_type == meshTypes::AModel)) {
			throw std::runtime_error("Error: Loaded Model requires File!");
		}
	}
	StaticMeshComponent(meshTypes _type, const std::string model) : meshType(_type) {
		if ((_type == meshTypes::Model) || (_type == meshTypes::OrthoMesh) || (_type == meshTypes::AModel)) {
			if (!model.empty()) {
				modelName = std::make_unique<std::string>(model);
			}
			else {
				throw std::runtime_error("Error: Model Name empty!");
			}
		}
		else {
			throw std::runtime_error("Error: Type not a loaded model!");
		}
	}
	std::unique_ptr<std::string> modelName = nullptr;
	meshTypes meshType;
	std::weak_ptr<Material> material;
};