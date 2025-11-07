#pragma once
#include "../DXFramework/DXF.h"
#include "Material.h"
#include<memory>
#include <unordered_map>

struct GeometryInstanceData {
	XMMATRIX worldMatrix;
	Material material;
	std::weak_ptr<BaseMesh> mesh;
};

class GeometryManager {

public:
	
	void CreateAModel();

private:

	std::unordered_map<std::string, std::unique_ptr<BaseMesh>> meshCache; // caches generated geometry
	std::vector<GeometryInstanceData> geometryInstances;
};