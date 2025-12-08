#pragma once
#ifndef _FILEHANDLER_H_
#define _FILEHANDLER_H_


#include <unordered_map>
#include <filesystem>
#include <unordered_set>
#include <sstream>
#include "assimp\Importer.hpp"      // C++ importer interface
#include "Converters.h"
#include <vector>
#include <Nlohmann/json.hpp>
#include <fstream>

namespace fs = std::filesystem;
class FileHandler {
	//inline static std::unordered_map<std::string, bool> scenes;

	//Constructor
	FileHandler() {

		//Build scene folder if does not exist
		fs::path SceneFolder = "res/scenes";
		if (!fs::exists(SceneFolder)) {
			fs::create_directory(SceneFolder);
		}
		LocateFiles(L"res/");

		updateVectors();
	}

	void updateVectors(bool onlyUpdateScene = false) {
		if (!onlyUpdateScene) {
			size_t imageNum = images.size();
			size_t modelNum = models.size();
			imageStringList.resize(imageNum);
			imageCharList.resize(imageNum);
			modelCharList.resize(modelNum);
			size_t index = 0;
			for (auto it : images) {
				imageStringList[index] = Converters::convert_from_wstring(it.first);
				imageCharList[index] = imageStringList[index].c_str();
				++index;
			}
			index = 0;
			for (auto& it : models) {
				modelCharList[index] = it.first.c_str();
				++index;
			}
		}
		size_t sceneNum = scenes.size();
		sceneCharList.resize(sceneNum);
		int index = 0;
		for (auto& it : scenes) {
			sceneCharList[index] = it.first.c_str();
			++index;
		}
	}

	~FileHandler() {

	}

	inline void LocateFiles(const wchar_t* _DirectoryPath) {
		const std::vector<std::string> supportedTextureExtensions = {
			".dds",
			".png",
			".jpg",
			".jpeg",
			".bmp",
			".tif",
			".tiff",
			".gif"
		};
		std::vector<std::string> supportedObjectExtensions;

		Assimp::Importer importer;

		std::string ObjectExtensionList;
		importer.GetExtensionList(ObjectExtensionList);

		std::stringstream ss(ObjectExtensionList);
		std::string token;

		while (std::getline(ss, token, ';')) {
			token.erase(std::remove(token.begin(), token.end(), '*'), token.end());

			// Trim whitespace just in case
			token.erase(std::remove_if(token.begin(), token.end(), ::isspace), token.end());

			if (!token.empty())
			{
				supportedObjectExtensions.push_back(token);
			}
		}



		std::string extension; //String for storing files extension type
		std::string stringName;      //String for storing files name
		std::wstring wstringName;
		//Recursively loop through directories
		for (const auto& entry : fs::recursive_directory_iterator(_DirectoryPath))
		{
			std::string directoryString = Converters::convert_from_wstring(std::wstring(_DirectoryPath));
			const auto path = entry.path();
			//Get current files extension
			extension = path.extension().string();
			//Get current files name
			stringName = path.stem().string();
			wstringName = path.stem().wstring();
			//If regular file
			if (entry.is_regular_file())
			{
				bool found = false;
				
				if (extension == ".json" || extension == ".bson")
				{
					if (scenes.contains(stringName)) {
						throw std::runtime_error("Error: Map: " + stringName + " is a duplicate!");
					}

					bool isBson = (extension == ".bson");
					scenes.emplace(stringName, std::make_pair(path.string(), isBson));
				}
				bool isModel = false;

				//if file type .gltf add to models map
				for (const auto& checkExtension : supportedObjectExtensions) {
					if (extension == checkExtension) {
						if (models.find(stringName) != models.end()) { throw std::runtime_error("Error: Model: " + stringName + " Is a duplicate!"); }
						else {
							std::filesystem::path relativePath = fs::relative(path, _DirectoryPath).parent_path();
							std::string inputPath = directoryString + (relativePath.empty() ? "" : (relativePath.string() + "/")) + stringName + extension;
							models.emplace(stringName, inputPath);
						}
						found = true;
						break;
					}
				}
				//Continue to next filetype already found
				if (found) { continue; }

				for (const auto& checkExtension : supportedTextureExtensions) {
					if (extension == checkExtension) {
						if (images.find(wstringName) != images.end()) { throw std::runtime_error("Error: Image: " + stringName + " Is a duplicate!"); }
						else {
							std::filesystem::path relativePath = fs::relative(path, _DirectoryPath).parent_path();
							std::wstring inputPath = std::wstring(_DirectoryPath) + relativePath.wstring() + L"/";
							inputPath = path.wstring();
							images.emplace(wstringName, inputPath);
							imageStringList.push_back(stringName);
						}
						break;
					}
				}
			}
		}
	}
public:

	inline static FileHandler& get() {
		static FileHandler handler;
		return handler;
	}

	//Delete copy/move constructors and assignment operators
	FileHandler(const FileHandler&) = delete;
	FileHandler(FileHandler&&) = delete;
	FileHandler& operator=(const FileHandler&) = delete;
	FileHandler& operator=(FileHandler&&) = delete;

	const std::vector<const char*>* getImageList() const { return &imageCharList; }
	const std::vector<const char*>* getModelList() const { return &modelCharList; }
	const std::vector<const char*>* getSceneList() const { return &sceneCharList; }

	inline bool loadSceneJson(const std::string& sceneName, nlohmann::json& json) {
		auto it = scenes.find(sceneName);
		if (it == scenes.end())
			return false;

		const std::string& path = it->second.first;
		bool isBson = it->second.second;

		std::ifstream file(path, isBson ? std::ios::binary : std::ios::in);
		if (!file.is_open())
			return false;
		
		if (isBson) {
			// Get file size
			file.seekg(0, std::ios::end);
			std::streamsize size = file.tellg();
			file.seekg(0, std::ios::beg);

			// Allocate buffer
			std::vector<uint8_t> buffer(size);

			// Read directly into uint8_t vector
			if (!file.read(reinterpret_cast<char*>(buffer.data()), size))
				return false;

			// Parse BSON
			json = nlohmann::json::from_bson(buffer);
		} else {
			file >> json;
		}
		return true;
	}
	
	inline bool saveSceneJson(const std::string& sceneName, const nlohmann::json& json, bool asBson) {
		std::string* path = nullptr;
		
		//If sceneName already exists
		auto it = scenes.find(sceneName);
		if (it != scenes.end()) {
			//Get file path of existing scene
			path = &it->second.first;
			if (it->second.second != asBson) {
				//Update format
				
				try {
					if (fs::exists(*path)) {
						fs::remove(*path);   // deletes the file
					}
				}
				catch (const fs::filesystem_error& e) {
					throw std::runtime_error(std::string("Error deleting file: ") + e.what()); throw std::runtime_error("Error could not delete file!");
				}
				it->second.second = asBson;
				*path = "res/scenes/" + sceneName + (asBson ? ".bson" : ".json");
			}
			
		} else {
			//Build new scene path
			std::string inputPath = "res/scenes/" + sceneName + (asBson ? ".bson" : ".json");

			// Insert new scene
			auto [iter, inserted] = scenes.emplace(sceneName, std::make_pair(inputPath, asBson));
			path = &iter->second.first;
			updateVectors(true);
		}

		//if path not found or correctly generated
		if (!path)
			return false;

		//If saving as binary json
		if (asBson) {
			//Convert to binary json
			std::vector<uint8_t> bsonData = nlohmann::json::to_bson(json);
			//Open and clear file
			std::ofstream file(*path, std::ios::binary | std::ios::trunc);
			//If file open successful
			if (!file.is_open()) return false;
			//Write binary to file
			file.write(reinterpret_cast<const char*>(bsonData.data()), bsonData.size());
		}
		else {	// is saving as json
			//Open and clear file
			std::ofstream file(*path, std::ios::trunc);
			//If open successful
			if (!file.is_open()) return false;
			//Write json to file
			file << json.dump(4);
		}
		return true;
	}
	
	//Find model in map
	inline const std::string& locateModel(const std::string& name) {
		auto model = models.find(name);
		if (model != models.end()) {
			return model->second;
		}
		return nullptr;
	}

	//Fine texture in map
	inline std::wstring* locateImage(const std::wstring& name) {
		auto image = images.find(name);
		if (image != images.end()) {
			return &image->second;
		}
		return nullptr;
	}
private:

	std::vector<std::wstring> supportedObjectExtensions;

	std::unordered_map<std::string, std::string> models;
	std::unordered_map<std::wstring, std::wstring> images;
	std::unordered_map<std::string, std::pair<std::string, bool>> scenes;

	std::vector<std::string> imageStringList;

	std::vector<const char*> imageCharList;
	std::vector<const char*> modelCharList;
	std::vector<const char*> sceneCharList;
};

#endif