//
// Created by Paul McGinley on 30/09/2024.
//

#pragma once // Inclusion sguard to prevent multiple includes

#include "models/TextureLibrary.h"
#include <filesystem>
#include <iostream>

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

    TextureEntry* GetRobeFrame_ptr(int lib, int cell) {
        return &Robes[lib].entries[cell];
    }
    TextureEntry* GetStaffFrame_ptr(int lib, int cell) {
        return &Staffs[lib].entries[cell];
    }
    TextureEntry* GetChestMonsterFrame_ptr(int cell) {
        return &ChestMonster.entries[cell];
    }
    TextureEntry* GetHeartImage_ptr(int cell) {
        return &Hearts.entries[cell];
    }

    std::vector<TextureLibrary> Robes;
    std::vector<TextureLibrary> Staffs;
    TextureLibrary ChestMonster = TextureLibrary("");
    TextureLibrary Hearts = TextureLibrary("");
    TextureLibrary BackgroundIslands = TextureLibrary("");

private:
    // Private constructor to prevent instancing
    AssetManager() = default;
};
