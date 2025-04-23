//
// Created by Paul McGinley on 30/09/2024.
//

#include "AssetManager.h"

// Initialize the static member
AssetManager* AssetManager::instance = nullptr;

// Define the constructor
AssetManager::AssetManager() {
        // Constructor implementation
}

// Define the singleton getter
AssetManager& AssetManager::getInstance() {
        if (!instance) {
                instance = new AssetManager();
        }
        return *instance;
}

// Define the global reference
AssetManager& asset_manager = AssetManager::getInstance();

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