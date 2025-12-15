#pragma once
#include "ShaderManager.h"
#include <optional>

struct resourceData {
	resourceData() {}
	resourceData(std::string moduleName, std::vector<std::pair<D3D11_SHADER_VERSION_TYPE, size_t>> resourceAllocationData)
		: moduleName(std::move(moduleName)), resourceAllocationData(std::move(resourceAllocationData)) {
	}

	std::string moduleName;
	std::vector<std::pair<D3D11_SHADER_VERSION_TYPE, size_t>> resourceAllocationData;
};


struct shaderData {
	shaderData(
		std::optional<const wchar_t*> pixelShader = std::nullopt,
		std::optional<const wchar_t*> vertexShader = std::nullopt,
		std::optional<const wchar_t*> geometryShader = std::nullopt,
		std::optional<const wchar_t*> hullShader = std::nullopt,
		std::optional<const wchar_t*> domainShader = std::nullopt,
		std::optional<const wchar_t*> computeShader = std::nullopt
	) {
		ps = pixelShader != std::nullopt ? *pixelShader : nullptr;
		vs = vertexShader != std::nullopt ? *vertexShader : nullptr;
		gs = geometryShader != std::nullopt ? *geometryShader : nullptr;
		hs = hullShader != std::nullopt ? *hullShader : nullptr;
		ds = domainShader != std::nullopt ? *domainShader : nullptr;
		cs = computeShader != std::nullopt ? *computeShader : nullptr;
	}
	const wchar_t* ps;
	const wchar_t* vs;
	const wchar_t* gs;
	const wchar_t* hs;
	const wchar_t* ds;
	const wchar_t* cs;
};

class GenericShader : public BaseShader {
public:
	GenericShader(
		ShaderManager* shaderManager, 
		ID3D11Device* device, HWND hwnd, 
		shaderData loadData, 
		std::vector<resourceData> moduleData) 
		: BaseShader(device, hwnd) 
	{
		//Resize module vector to correct size
		modules.resize(moduleData.size());
		//Add modules to shader and correct resource bindings
		for (size_t i = 0; i < moduleData.size(); ++i) {
			modules[i] = shaderManager->getShaderModuleID(moduleData[i].moduleName);
			auto& AllocationData = moduleData[i].resourceAllocationData;
			for (const auto& it : AllocationData) {
				this->resourceAllocationData.push_back(std::make_pair(it.first, std::make_pair(i, it.second)));
			}
		}

		//LoadShaders
		if (!loadData.ps || !loadData.vs) { throw std::runtime_error("Error cannot instantiate without a vertex and a pixel shader!"); }
		initShader(loadData.vs, loadData.ps);
		if (loadData.gs) { loadGeometryShader(loadData.gs); }
		if (loadData.hs) { loadHullShader(loadData.hs); }
		if (loadData.ds) { loadDomainShader(loadData.ds); }
		if (loadData.cs) { loadComputeShader(loadData.cs); }
	}

	~GenericShader() {
		// Release the layout.
		if (layout)
		{
			layout->Release();
			layout = 0;
		}

		//Release base shader components
		BaseShader::~BaseShader();
	}

	void initShader(const wchar_t* vertexShader, const wchar_t* pixelShader) {
		loadVertexShader(vertexShader);
		loadPixelShader(pixelShader);
	}

	void setResources(ID3D11DeviceContext* deviceContext) {
		for (const auto& resource : resourceAllocationData) {
			D3D11_SHADER_VERSION_TYPE shaderType = resource.first;
			size_t moduleIndex = resource.second.first;
			size_t startingRegister = resource.second.second;
			modules[moduleIndex]->module->setResources(shaderType, deviceContext, startingRegister);
		}
	}

private:
	std::vector<ModuleInstance> modules;
	std::vector<std::pair<D3D11_SHADER_VERSION_TYPE, std::pair<size_t, size_t>>> resourceAllocationData;
};