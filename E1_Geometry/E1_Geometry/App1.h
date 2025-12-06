// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "../DXFramework/DXF.h"
#include "TexturedLightShader.h"


class App1 : public BaseApplication
{
	enum Pipelines {
		BASICLIGHTING,
		BASICLIGHTINGWITHBLOOM,
		SHADOWS,
		SHADOWSWITHBLOOM
	};

	inline static std::vector<const char*> PipelinesStrings = {
		"Basic Lighting",
		"Basic Lighting With Bloom",
		"Shadows",
		"Shadows With Bloom"
	};
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	void addModuleConstructors(HWND hwnd, ID3D11Device* device);
	void addShaderConstructors(HWND hwnd, ID3D11Device* device);

	void initShadowPasses(HWND hwnd, int screenWidth, int screenHeight);
	void initBasicLightPasses(HWND hwnd, int screenWidth, int screenHeight);
	void initBloompasses(HWND hwnd, int screenWidth, int screenHeight, const char* previousPass);

	void initBasicLightsPipeline(HWND hwnd, int screenWidth, int screenHeight);
	void initShadowLightsPipeline(HWND hwnd, int screenWidth, int screenHeight);
	void initBloomLightsPipeline(HWND hwnd, int screenWidth, int screenHeight);
	void initBloomShadowPipeline(HWND hwnd, int screenWidth, int screenHeight);

	void loadScene(const std::string& sceneName);
	void saveScene(const std::string& sceneName);

	void selectPipeline(Pipelines pipeline);
	

	bool frame();

protected:
	bool render();
	void gui();
private:
	int selectedScene;
	int selectedPipeline;

	char SceneNameBuffer[128];
	bool saveAsBson; 
	std::string sceneName;
};

#endif