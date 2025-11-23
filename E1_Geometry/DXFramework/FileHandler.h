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

namespace fs = std::filesystem;
class FileHandler {
	std::vector<std::wstring> supportedObjectExtensions;

	std::unordered_map<std::string, std::wstring> models;
	std::unordered_map<std::wstring, std::wstring> images;

	std::vector<const char*> imageList;
	std::vector<const char*> modelList;
	//inline static std::unordered_map<std::string, bool> scenes;

	FileHandler() {
		LocateFiles(L"res/");
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
			//Get current files extension
			extension = entry.path().extension().string();
			//Get current files name
			stringName = entry.path().stem().string();
			wstringName = entry.path().stem().wstring();
			//If regular file
			if (entry.is_regular_file())
			{
				bool found = false;
				/*
				if (extension == ".json" || extension == ".bson")
				{
					if (maps.find(name) != maps.end()) { SKTBD_APP_CRITICAL("Error: Map: " + name + " Is a duplicate!"); }
					else {
						maps.emplace(name, extension == ".bson");
					}
					continue;
				}
				*/
				bool isModel = false;

				//if file type .gltf add to models map
				for (const auto& checkExtension : supportedObjectExtensions) {
					if (extension == checkExtension) {
						if (models.find(stringName) != models.end()) { throw std::runtime_error("Error: Model: " + stringName + " Is a duplicate!"); }
						else {
							std::filesystem::path relativePath = fs::relative(entry.path(), _DirectoryPath).parent_path();
							std::wstring inputPath = std::wstring(_DirectoryPath) + relativePath.wstring() + L"/";
							models.emplace(stringName, inputPath);
							modelList.push_back(stringName.c_str());
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
							std::filesystem::path relativePath = fs::relative(entry.path(), _DirectoryPath).parent_path();
							std::wstring inputPath = std::wstring(_DirectoryPath) + relativePath.wstring() + L"/";
							inputPath = entry.path().wstring();
							images.emplace(wstringName, inputPath);
							imageList.push_back(stringName.c_str());
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

	const std::vector<const char*>* getImageList() const { return &imageList; }
	const std::vector<const char*>* getModelList() const { return &modelList; }


	inline std::wstring* locateModel(const std::string& name) {
		auto model = models.find(name);
		if (model != models.end()) {
			return &model->second;
		}
		return nullptr;
	}

	inline std::wstring* locateImage(const std::wstring& name) {
		auto image = images.find(name);
		if (image != images.end()) {
			return &image->second;
		}
		return nullptr;
	}
};

#endif