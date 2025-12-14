
#include "RenderPass.h"
#include "MatrixDataModule.h"
#include "LightDepthDataBuffer.h"
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
		texelSize = XMFLOAT2(1.f / static_cast<float>(inputData.sWidth), 1.f / static_cast<float>(inputData.sHeight));

		matrixDataModule = shaderManager->getShaderModuleID("MatrixDataModule");
		lightDepthDataModule = shaderManager->getShaderModuleID("LightDepthDataModule");
		heightMapDataModule = shaderManager->getShaderModuleID("HeightMapDataModule");
		heightMapTextureModule = shaderManager->getShaderModuleID("TextureDataModule3");

		DepthShader = shaderManager->getGeometryShader("DepthShader");
		HeightMapDepthShader = shaderManager->getGeometryShader("HeightDepthShader");
		
		numPointLights = 0;
		numNonPointLights = 0;
		maxPointLights = 5;
		maxNonPointLights = 5;
		spotLightAspect = inputData.sWidth / inputData.sHeight;

		

		D3D11_RASTERIZER_DESC rasterDesc;
		rasterDesc.AntialiasedLineEnable = false;
		rasterDesc.CullMode = D3D11_CULL_FRONT;
		rasterDesc.DepthBias = 0;
		rasterDesc.DepthBiasClamp = 0.0f;
		rasterDesc.DepthClipEnable = true;
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rasterDesc.FrontCounterClockwise = true;
		rasterDesc.MultisampleEnable = false;
		rasterDesc.ScissorEnable = false;
		rasterDesc.SlopeScaledDepthBias = 0.0f;

		renderer->getDevice()->CreateRasterizerState(&rasterDesc, rsCullFront.GetAddressOf());
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
		ID3D11RasterizerState* previousRasteriserState;

		deviceContext->RSGetState(&previousRasteriserState);

		deviceContext->RSSetState(rsCullFront.Get());

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
		std::shared_ptr<LightDepthDataBuffer> depthDataModule = dynamic_pointer_cast<LightDepthDataBuffer>(lightDepthDataModule->module);

		auto GeometryRenderFunction = [&](bool isPointLight, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix) {
			instanceManager->forEachMesh([&](const size_t& ID, GeometryData* instance) {
				MeshType type = instance->mesh->type;
				if (type != MeshType::ORTHO && type != MeshType::POINT)
				{
					Material* mat = instance->mat;
					bool isHeightMap = (mat->HeightMapData != nullptr);
					std::shared_ptr<BaseShader> shader;
					if (isHeightMap) {
						deviceContext->RSSetState(previousRasteriserState);
						std::shared_ptr<HeightMapDataModule> mapDataModule 
							= std::dynamic_pointer_cast<HeightMapDataModule>(heightMapDataModule->module);
						std::shared_ptr<TextureDataModule> textureModule 
							= std::dynamic_pointer_cast<TextureDataModule>(heightMapTextureModule->module);
						mapDataModule->setModuleParamaters(deviceContext, mat, instance);
						textureModule->setModuleParamaters(deviceContext, mat->HeightMapData->HeightTexture);

						shader = HeightMapDepthShader->shader;
						
					}
					else {
						shader = DepthShader->shader;
					}
					matrixModule->setModuleParamaters(deviceContext, instance, projectionMatrix, viewMatrix);
					BaseMesh* mesh = instance->mesh->mesh.get();
					mesh->sendData(deviceContext);
					shader->setResources(deviceContext);
					shader->render(deviceContext, mesh->getIndexCount());
					if(isHeightMap) { deviceContext->RSSetState(rsCullFront.Get()); }
				}
			});

		};

		for (std::pair<size_t, Light*> light : pointLights) {
			depthDataModule->setModuleParamaters(deviceContext, light.second);
			for (int f = 0; f < 6; f++) {
				cubeMapArray.BindDsvAndSetNullRenderTarget(deviceContext, pointIndex, f);
				GeometryRenderFunction(true, light.second->getViewMatrix(f), light.second->getProjectionMatrix());
			}
			++pointIndex;
		}

		for (std::pair<size_t, Light*> light : spotLights) {
			depthDataModule->setModuleParamaters(deviceContext, light.second);
			shadowMapArray.BindDsvAndSetNullRenderTarget(deviceContext, nonPointIndex);
			GeometryRenderFunction(false, light.second->getViewMatrix(), light.second->getProjectionMatrix());
			++nonPointIndex;
		}

		for (std::pair<size_t, Light*> light : directionalLights) {
			depthDataModule->setModuleParamaters(deviceContext, light.second);
			shadowMapArray.BindDsvAndSetNullRenderTarget(deviceContext, nonPointIndex);
			GeometryRenderFunction(false, light.second->getViewMatrix(), light.second->getProjectionMatrix());
			++nonPointIndex;
		}

		deviceContext->RSSetState(previousRasteriserState);
		previousRasteriserState->Release();
	};
	ShadowMapArray* getShadowMapArray() { return &shadowMapArray; }
	CubeMapArray* getCubeMapArray() { return &cubeMapArray; }
	const XMFLOAT2* getTexelSize() { return &texelSize; }

	virtual void toJson(nlohmann::json& json) {}
	virtual void fromJson(const nlohmann::json& json) {}
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
	ModuleInstance lightDepthDataModule; //lights depth data module
	ModuleInstance heightMapDataModule;
	ModuleInstance heightMapTextureModule;

	ShaderInstance DepthShader;
	ShaderInstance HeightMapDepthShader;

	int numPointLights;
	int numNonPointLights;
	int maxPointLights;
	int maxNonPointLights;

	float spotLightAspect;

	XMFLOAT2 texelSize;

	static constexpr float expandThreshold = 1.0f;   // 100% of capacity
	static constexpr float shrinkThreshold = 0.25f;  // 25% of capacity

	ComPtr<ID3D11RasterizerState> rsCullFront;
};