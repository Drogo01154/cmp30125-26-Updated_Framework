
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

	void Render(ID3D11DeviceContext* deviceContext, ID3D11Device* device) {
		if (shaderManager->isModuleDirtyflagSet(DirtyModuleFlags::LIGHTNUMCHANGED)) {
			updateSizes(device);
		}

		int pointIndex = 0;
		int nonPointIndex = 0;

		std::shared_ptr<MatrixDataModule> matrixModule = dynamic_pointer_cast<MatrixDataModule>(matrixDataModule->module);

		auto GeometryRenderFunction = [&](const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix) {
			instanceManager->forEachMesh([&](const size_t& ID, GeometryData* instance) {
				std::shared_ptr<BaseShader> shader = instance->mat->shader->shader;
				const std::string& shaderType = instance->mat->shader->name;
				matrixModule->setModuleParamaters(deviceContext, instance, projectionMatrix, viewMatrix);
				BaseMesh* mesh = instance->mesh->mesh.get();
				mesh->sendData(deviceContext);
				shader->setResources(deviceContext);
				shader->render(deviceContext, mesh->getIndexCount());
				});
			};

		instanceManager->forEachLight([&](const size_t& ID, Light* light) {
			lightTypes lightType = light->getType();
			if (lightType == lightTypes::point) {
				for (int f = 0; f < 6; f++) {
					cubeMapArray.BindDsvAndSetNullRenderTarget(deviceContext, pointIndex, f);
					GeometryRenderFunction(light->getViewMatrix(f), light->getProjectionMatrix());
				}
				++pointIndex;
			}
			else {
				shadowMapArray.BindDsvAndSetNullRenderTarget(deviceContext, nonPointIndex);
				instanceManager->forEachMesh([&](const size_t& ID, GeometryData* instance) {
					const std::string& shaderType = instance->mat->shader->name;
					GeometryRenderFunction(
						light->getViewMatrix(), 
						lightType == lightTypes::directional 
							? light->getOrthoMatrix() 
							: light->getProjectionMatrix());
					});
				++nonPointIndex;
			}
			
		});
	};
private:
	ShadowMapArray shadowMapArray;
	CubeMapArray cubeMapArray;

	D3D* renderer;
	ShaderManager* shaderManager;
	InstanceManager* instanceManager;
	ModuleInstance matrixDataModule;	//Matrix Data Module;

	int numPointLights;
	int numNonPointLights;
	int maxPointLights;
	int maxNonPointLights;

	static constexpr float expandThreshold = 1.0f;   // 100% of capacity
	static constexpr float shrinkThreshold = 0.25f;  // 25% of capacity
};