
#include "RenderPass.h"
#include "MatrixDataModule.h"

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
		shader = shaderManager->getGeometryShader("DepthShader");
		matrixDataModule = shaderManager->getShaderModuleID("MatrixDataModule");
		numPointLights = 0;
		numNonPointLights = 0;
		maxPointLights = 5;
		maxNonPointLights = 5;
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
		else if ((numPointLights > 5) && numLights < maxPointLights * shrinkThreshold) {
			maxPointLights /= 2;
			cubeMapArray.resize(device, maxPointLights);
		}

		if (numNonPointLights > maxNonPointLights * expandThreshold) {
			maxNonPointLights *= 2;
			shadowMapArray.resize(device, maxNonPointLights);
		}
		else if ((maxNonPointLights > 5) && numLights < maxNonPointLights * shrinkThreshold) {
			maxNonPointLights /= 2;
			shadowMapArray.resize(device, maxNonPointLights);
		}
	}

	void Render(ID3D11DeviceContext* deviceContext, ID3D11Device* device) {
		if (shaderManager->isModuleDirtyflagSet(DirtyModuleFlags::LIGHTNUMCHANGED)) {
			updateSizes(device);
		}

		int pointIndex = 0;
		int nonPointIndex = 0;

		instanceManager->forEachLight([&](const size_t& ID, Light* light) {
			lightTypes lightType = light->getType();
			XMVECTOR lightPos = light->getGlobalPosition()
			if (lightType == lightTypes::point) {
				for (int f = 0; f < 6; f++) {
					cubeMapArray.BindDsvAndSetNullRenderTarget(deviceContext, pointIndex, f);
					
					XMMATRIX view = XMMatrixLookAtLH()
					view[face] = XMMatrixLookAtLH(
						position,                       // Camera position
						position + directions[face],    // Look at direction
						up[face]                        // Up vector
					);
					
					
					instanceManager->forEachMesh([&](const size_t& ID, GeometryData& meshData) {

						});
				}
				++pointIndex;
			}
			else {
				shadowMapArray.BindDsvAndSetNullRenderTarget(deviceContext, nonPointIndex);
				instanceManager->forEachMesh([&](const size_t& ID, GeometryData& meshData) {
					XMMATRIX viewMatrix;
					if (lightType == lightTypes::directional) {
						viewMatrix = light->getOrthoMatrix();
					}
					else {
						viewMatrix = light->getProjectionMatrix();
					}
					});
				++nonPointIndex;
			}
			
		});

		/*
		textureOutput->setRenderTarget(renderer->getDeviceContext());
		textureOutput->clearRenderTarget(renderer->getDeviceContext(), 0.39f, 0.58f, 0.92f, 1.0f);

		std::shared_ptr<MatrixDataModule> matrixModule = dynamic_pointer_cast<MatrixDataModule>(matrixDataModule->module);
		instanceManager->forEachMesh([&](const size_t& ID, GeometryData* instance) {
			if (instance->mat.IsValid()) {
				std::shared_ptr<BaseShader> shader = instance->mat->shader->shader;
				const std::string& shaderType = instance->mat->shader->name;

				matrixModule->setModuleParamaters(deviceContext, instance, renderer->getProjectionMatrix());
				BaseMesh* mesh = instance->mesh->mesh.get();
				mesh->sendData(deviceContext);
				shader->setResources(deviceContext);
				shader->render(deviceContext, mesh->getIndexCount());
			}
			});
		*/
	};
private:
	ShadowMapArray shadowMapArray;
	CubeMapArray cubeMapArray;

	D3D* renderer;
	ShaderManager* shaderManager;
	InstanceManager* instanceManager;
	ModuleInstance matrixDataModule;	//Matrix Data Module;
	ShaderInstance shader;

	int numPointLights;
	int numNonPointLights;
	int maxPointLights;
	int maxNonPointLights;

	static constexpr float expandThreshold = 1.0f;   // 100% of capacity
	static constexpr float shrinkThreshold = 0.25f;  // 25% of capacity

	XMFLOAT3 directions[6] = {
	{ 1, 0, 0 },
	{-1, 0, 0 },
	{ 0, 1, 0 },
	{ 0,-1, 0 },
	{ 0, 0, 1 },
	{ 0, 0,-1 }
	};

	XMFLOAT3 ups[6] = {
		{ 0,-1, 0 },
		{ 0,-1, 0 },
		{ 0, 0, 1 },
		{ 0, 0,-1 },
		{ 0,-1, 0 },
		{ 0,-1, 0 }
	};
};