//
// Created by Paul McGinley on 23/04/2025.
//

#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include <map>
#include <vector>
#include "models/TextureLibrary.h"
#include "models/LevelObject/OLibrary.h"

// Forward declarations to avoid circular dependencies
class WMap;

class AssetManager {

public:
        // Delete copy constructor and assignment
        AssetManager(const AssetManager&) = delete;
        AssetManager& operator=(const AssetManager&) = delete;

        // Static method to get instance
        static AssetManager& GetInstance();

        // Method declarations only, no implementations here
        // TODO: Deprecate these in favor of the TextureLibraries map
        TextureEntry* getRobeFrame_ptr(int lib, int cell);
        TextureEntry* getStaffFrame_ptr(int lib, int cell);
        TextureEntry* getChestMonsterFrame_ptr(int cell);
        TextureEntry* getHeartImage_ptr(int cell);
        TextureEntry* getProgramUseImage_ptr(int cell);

        // Data members
        // TODO: Deprecate these in favor of the TextureLibraries map
        std::vector<TextureLibrary> robes;
        std::vector<TextureLibrary> staffs;
        TextureLibrary chestMonster = TextureLibrary("");
        TextureLibrary hearts = TextureLibrary("");
        TextureLibrary backgroundIslands = TextureLibrary("");
        TextureLibrary programUsage = TextureLibrary("");


        // Only textures required for gameplay will be loaded
        std::map<std::string, std::unique_ptr<TextureLibrary>> TextureLibraries;

        // As these files are tiny, all will be loaded into memory
        std::map<std::string, OLibrary> ObjectLibraries;
        std::map<std::string, std::unique_ptr<WMap>> Maps;

        void LoadLibrary(std::string fileName);

private:
        // Private constructor to prevent instancing
        AssetManager();

        // Create a static instance of the AssetManager
        static AssetManager* instance;
};

// This line declares a global variable named 'asset_manager'.
// The 'extern' keyword tells the compiler that this variable is defined
// (actually created and initialized) in another .cpp file.
// This allows different parts of the program to access the same single
// instance of the AssetManager through this 'asset_manager' reference.
extern AssetManager& asset_manager;

#endif //ASSETMANAGER_H
