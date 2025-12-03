
#include "RenderPass.h"
#include "MatrixDataModule.h"
#include "PointLightDataModule.h"
#include "HeightMapDataModule.h"

struct DepthPassInput {
	InstanceManager* instanceManager;
	ShaderManager* shaderManager;
	D3D* renderer;
	int sWidth;
	int sHeight;
};

class DepthPass : public renderPass {
public:
	DepthPass(size_t stage,
		passDependancies& deps,
		const DepthPassInput& inputData)
		: renderPass(stage, deps),
		shadowMapArray(inputData.renderer->getDevice(), inputData.sWidth, inputData.sHeight, 5),
		cubeMapArray(inputData.renderer->getDevice(), inputData.sWidth, inputData.sHeight, 5),
		instanceManager(inputData.instanceManager),
		shaderManager(inputData.shaderManager),
		renderer(inputData.renderer) 
	{
		matrixDataModule = shaderManager->getShaderModuleID("MatrixDataModule");
		pointLightDataModule = shaderManager->getShaderModuleID("PointLightDataModule");
		heightMapDataModule = shaderManager->getShaderModuleID("HeightMapDataModule");
		heightMapTextureModule = shaderManager->getShaderModuleID("HeightMapTextureDataModule");

		NonPointDepthShader = shaderManager->getGeometryShader("SMDepthShader");
		PointDepthShader = shaderManager->getGeometryShader("CMDepthShader");
		NonPointHeightMapDepthShader = shaderManager->getGeometryShader("SMHeightDepthShader");
		PointHeightMapDepthShader = shaderManager->getGeometryShader("CMHeightDepthShader");

		
		numPointLights = 0;
		numNonPointLights = 0;
		maxPointLights = 5;
		maxNonPointLights = 5;
		spotLightAspect = inputData.sWidth / inputData.sHeight;
	}
	
	void updateProjectionMatrices() {
		instanceManager->forEachLight([&](const size_t& ID, Light* light) {
			light->generateProjectionMatrix(spotLightAspect);
			});
	}

	void updateViewMatrices() {
		instanceManager->forEachLight([](const size_t& ID, Light* light) {
			light->generateViewMatrix();
			});
	}

	void updateSizes(ID3D11Device* device) {
		numPointLights = 0;
		numNonPointLights = 0;
		instanceManager->forEachLight([&](const size_t& ID, Light* light) {
			if (light->getType() == lightTypes::point) {
				numPointLights++;
			} else {
				numNonPointLights++;
			}
			});

		if (numPointLights > maxPointLights * expandThreshold) {
			maxPointLights *= 2;
			cubeMapArray.resize(device, maxPointLights);
		}
		else if ((numPointLights > 5) && numPointLights < maxPointLights * shrinkThreshold) {
			maxPointLights /= 2;
			cubeMapArray.resize(device, maxPointLights);
		}

		if (numNonPointLights > maxNonPointLights * expandThreshold) {
			maxNonPointLights *= 2;
			shadowMapArray.resize(device, maxNonPointLights);
		}
		else if ((numNonPointLights > 5) && numNonPointLights < maxNonPointLights * shrinkThreshold) {
			maxNonPointLights /= 2;
			shadowMapArray.resize(device, maxNonPointLights);
		}
	}

	const std::vector<std::pair<size_t, Light*>>* getPointLights() { return &pointLights; }
	const std::vector<std::pair<size_t, Light*>>* getSpotLights() { return &spotLights; }
	const std::vector<std::pair<size_t, Light*>>* getDirectionalLights() { return &directionalLights; }
	const std::vector<int>* getLightIndexes() { return &lightIndexes; }
	void updateVectors() {
		pointLights.clear();
		directionalLights.clear();
		spotLights.clear();
		lightIndexes.clear();
		size_t index = 0;
		int nonPointIndex = 0;
		/*
			Loop over lights and stoe indexes for shadow maps
		*/
		instanceManager->forEachLight([&](const size_t& ID, Light* light) {
			lightTypes lightType = light->getType();
			switch (lightType) {
			case lightTypes::directional:
				directionalLights.push_back({index, light});
				lightIndexes.push_back(nonPointIndex);
				++nonPointIndex;
				break;
			case lightTypes::point:
				pointLights.push_back({ index, light });
				break;
			case lightTypes::spot:
				spotLights.push_back({ index, light });
				lightIndexes.push_back(nonPointIndex);
				++nonPointIndex;
				break;
			default: 
				throw std::runtime_error("Error: light type does not exist!");
			}
			++index;
		});
	}

	void Render(ID3D11DeviceContext* deviceContext, ID3D11Device* device) {
		bool lightNumChanged = shaderManager->isModuleDirtyflagSet(DirtyModuleFlags::LIGHTNUMCHANGED);
		bool projectionChanged = shaderManager->isModuleDirtyflagSet(DirtyModuleFlags::LIGHTPROJECTIONCHANGED);
		bool lightDataChanged = shaderManager->isModuleDirtyflagSet(DirtyModuleFlags::LIGHTSDATACHANGED);
		if (lightNumChanged) {
			updateSizes(device);
			updateVectors();
		}
		if(lightNumChanged || lightDataChanged) { updateViewMatrices(); }
		if(lightNumChanged || projectionChanged) { updateProjectionMatrices(); }

		int pointIndex = 0;
		int nonPointIndex = 0;

		std::shared_ptr<MatrixDataModule> matrixModule = dynamic_pointer_cast<MatrixDataModule>(matrixDataModule->module);
		std::shared_ptr<PointLightDataModule> pointLightModule = dynamic_pointer_cast<PointLightDataModule>(pointLightDataModule->module);

		auto GeometryRenderFunction = [&](bool isPointLight, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix) {
			instanceManager->forEachMesh([&](const size_t& ID, GeometryData* instance) {
				MeshType type = instance->mesh->type;
				if (type != MeshType::ORTHO && type != MeshType::POINT)
				{
					Material* mat = instance->mat;
					std::shared_ptr<BaseShader> shader;
					if (mat->HeightMapData) {
						std::shared_ptr<HeightMapDataModule> mapDataModule 
							= std::dynamic_pointer_cast<HeightMapDataModule>(heightMapDataModule->module);
						std::shared_ptr<TextureDataModule> textureModule 
							= std::dynamic_pointer_cast<TextureDataModule>(heightMapTextureModule->module);
						mapDataModule->setModuleParamaters(deviceContext, mat, instance);
						textureModule->setModuleParamaters(deviceContext, mat->HeightMapData->HeightTexture);

						shader = (isPointLight 
							? this->PointHeightMapDepthShader->shader 
							: this->NonPointHeightMapDepthShader->shader);
					}
					else {
						shader = (isPointLight 
							? this->PointDepthShader->shader 
							: this->NonPointDepthShader->shader);
					}
					matrixModule->setModuleParamaters(deviceContext, instance, projectionMatrix, viewMatrix);
					BaseMesh* mesh = instance->mesh->mesh.get();
					mesh->sendData(deviceContext);
					shader->setResources(deviceContext);
					shader->render(deviceContext, mesh->getIndexCount());
				}
			});

		};

		for (std::pair<size_t, Light*> light : pointLights) {
			pointLightModule->setModuleParamaters(deviceContext, light.second->getGlobalPosition(), 50.f);
			for (int f = 0; f < 6; f++) {
				cubeMapArray.BindDsvAndSetNullRenderTarget(deviceContext, pointIndex, f);
				GeometryRenderFunction(true, light.second->getViewMatrix(f), light.second->getProjectionMatrix());
			}
			++pointIndex;
		}

		for (std::pair<size_t, Light*> light : spotLights) {
			shadowMapArray.BindDsvAndSetNullRenderTarget(deviceContext, nonPointIndex);
			GeometryRenderFunction(false, light.second->getViewMatrix(), light.second->getProjectionMatrix());
			++nonPointIndex;
		}

		for (std::pair<size_t, Light*> light : directionalLights) {
			shadowMapArray.BindDsvAndSetNullRenderTarget(deviceContext, nonPointIndex);
			GeometryRenderFunction(false, light.second->getViewMatrix(), light.second->getProjectionMatrix());
			++nonPointIndex;
		}
	};
	ShadowMapArray* getShadowMapArray() { return &shadowMapArray; }
	CubeMapArray* getCubeMapArray() { return &cubeMapArray; }
private:
	ShadowMapArray shadowMapArray;
	CubeMapArray cubeMapArray;

	std::vector<std::pair<size_t, Light*>> pointLights;
	std::vector<std::pair<size_t, Light*>> directionalLights;
	std::vector<std::pair<size_t, Light*>> spotLights;

	std::vector<int> lightIndexes;

	D3D* renderer;
	ShaderManager* shaderManager;
	InstanceManager* instanceManager;
	ModuleInstance matrixDataModule;	//Matrix Data Module
	ModuleInstance pointLightDataModule; //Point light module
	ModuleInstance heightMapDataModule;
	ModuleInstance heightMapTextureModule;

	ShaderInstance NonPointDepthShader;
	ShaderInstance NonPointHeightMapDepthShader;
	ShaderInstance PointDepthShader;
	ShaderInstance PointHeightMapDepthShader;

	int numPointLights;
	int numNonPointLights;
	int maxPointLights;
	int maxNonPointLights;

	float spotLightAspect;

	static constexpr float expandThreshold = 1.0f;   // 100% of capacity
	static constexpr float shrinkThreshold = 0.25f;  // 25% of capacity
};