//
// Created by Paul McGinley on 30/09/2024.
//

#pragma once // Inclusion sguard to prevent multiple includes

#include <map>
#include "models/LevelObject/OLibrary.h"
#include "models/TextureLibrary.h"

#include "models/MapObject/WMap.h"

class AssetManager {
public:
        // Method to get the instance of the singleton
        static AssetManager& getInstance() {
            static AssetManager instance; // Guaranteed to be destroyed.
            // Instantiated on first use.
            return instance;
        }

        // Delete copy constructor and assignment operator to prevent copies
        AssetManager(const AssetManager&) = delete;
        void operator=(const AssetManager&) = delete;

        TextureEntry* getRobeFrame_ptr(const int lib, const int cell) {
                return &robes[lib].entries[cell];
        }
        TextureEntry* getStaffFrame_ptr(const int lib, const int cell) {
                return &staffs[lib].entries[cell];
        }
        TextureEntry* getChestMonsterFrame_ptr(const int cell) {
                return &chestMonster.entries[cell];
        }
        TextureEntry* getHeartImage_ptr(const int cell) {
                return &hearts.entries[cell];
        }
        TextureEntry* getProgramUseImage_ptr(const int cell) {
                return &programUsage.entries[cell];
        }

        std::vector<TextureLibrary> robes;
        std::vector<TextureLibrary> staffs;
        TextureLibrary chestMonster = TextureLibrary("");
        TextureLibrary hearts = TextureLibrary("");
        TextureLibrary backgroundIslands = TextureLibrary("");
        TextureLibrary programUsage = TextureLibrary("");

        std::map<std::string, OLibrary> ObjectLibraries;
        std::map<std::string, WMap> Maps;

private:
        // Private constructor to prevent instancing
        AssetManager() = default;
};