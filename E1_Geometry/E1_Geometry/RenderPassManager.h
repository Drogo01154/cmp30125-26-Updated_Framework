#pragma once
#include "RenderPass.h"
#include <memory>
/*
	Class for managing render passes
*/
class RenderPassManager
{
public:
	RenderPassManager() {};

	void addPass(std::unique_ptr<renderPass> pass) {
		renderPasses.push_back(std::move(pass));
	}

	void renderAll() {
		for (auto& pass : renderPasses) {
			pass->Render();
		}
	}

	void removePass(size_t  ID) {
		if (ID < renderPasses.size()) {
			renderPasses.erase(renderPasses.begin() + ID);
		}
		else {
			throw std::runtime_error("Error: Pass " + std::to_string(ID) + " does not exist!");
		}
	}

	renderPass* getPass(size_t ID) {
		if (ID < renderPasses.size()) {
			return renderPasses[ID].get();
		}
		return nullptr;
	}

	void clearPasses() { renderPasses.clear(); }
private:
	std::vector<std::unique_ptr<renderPass>> renderPasses;
};