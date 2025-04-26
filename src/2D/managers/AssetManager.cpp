//
// Created by Paul McGinley on 30/09/2024.
//

#include "AssetManager.h"
#include "models/MapObject/WMap.h"

#include <iostream>

// Initialize the static member
AssetManager *AssetManager::instance = nullptr;

// Define the constructor
AssetManager::AssetManager() {
        // Constructor implementation
}

// Define the singleton getter
AssetManager& AssetManager::GetInstance() {
        if (!instance) {
                instance = new AssetManager();
        }
        return *instance;
}

// Define the global reference
AssetManager& asset_manager = AssetManager::GetInstance();

// Implementation of member functions
TextureEntry* AssetManager::getRobeFrame_ptr(int lib, int cell) {
        return &robes[lib].entries[cell];
}

TextureEntry* AssetManager::getStaffFrame_ptr(int lib, int cell) {
        return &staffs[lib].entries[cell];
}

TextureEntry* AssetManager::getChestMonsterFrame_ptr(int cell) {
        return &chestMonster.entries[cell];
}

TextureEntry* AssetManager::getHeartImage_ptr(int cell) {
        return &hearts.entries[cell];
}

TextureEntry* AssetManager::getProgramUseImage_ptr(int cell) {
        return &programUsage.entries[cell];
}

// Loads a library to memory without the image data
// I am creating the libraries as unique_ptrs to ensure they are deleted when the program ends ;)
// This also prevents them from going out of scope when the function ends
void AssetManager::LoadLibrary(std::string fileName) {
        // Check if the map entry already exists
        if (TextureLibraries.contains(fileName)) {
                return; // Library exists, no need to load again
        }

        // Create and load the library
        auto library = std::make_unique<TextureLibrary>(fileName);
        if (library->entryCount == 0) {
                std::cerr << "Failed to load library: " << fileName << std::endl;
                return;
        }

        fileName = fileName.substr(fileName.find_last_of("/\\") + 1);   // Get the file name
        fileName = fileName.substr(0, fileName.find_last_of('.'));      // Drop the extension

        // Add the library to the map
        TextureLibraries[fileName] = std::move(library); // std::move to transfer ownership
        std::cout << "Loaded library: " << fileName << std::endl;
}