//
// Created by Paul McGinley on 30/09/2024.
//

#include "AssetManager.h"

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

// Loads a library in its entirety
// This will be used for single set images (Armours, weapons, monsters, etc.)
// I am creating the libraries as unique_ptrs to ensure they are deleted when the program ends ;)
// This also prevents them from going out of scope when the function ends
void AssetManager::LoadLibrary(std::string fileName) {
        // Check if the map entry already exists
        if (TextureLibraries.contains(fileName)) {
                return; // Library exists, no need to load again

                // Future Paul: If we ever need to reload the library, uncomment this and comment the return above
                // TextureLibraries.erase(fileName); // (Automatically deletes the old object)
                // std::cout << "Removed library: " << fileName << std::endl;
        }

        // Create and load the library
        auto library = std::make_unique<TextureLibrary>(fileName);
        if (library->entries.empty()) {
                std::cerr << "Failed to load library: " << fileName << std::endl;
                return;
        }

        // Add the library to the map
        TextureLibraries[fileName] = std::move(library); // std::move to transfer ownership
        std::cout << "Loaded library: " << fileName << std::endl;
}

// Load a library with only the specified indices (image entries from the library)
// This will be used for pulling map specific images from the library
// The library will be initialized with the full size of the library, but only the specified indices will be loaded
// This is to save memory and loading time and for map switching it will save recreating the library
void AssetManager::LoadLibraryImages(std::string fileName, std::vector<int> indices) {
        // TODO: Refactor TextureLibrary loader to allow for partial loading
}