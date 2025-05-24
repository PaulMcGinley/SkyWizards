//
// Created by Paul McGinley on 30/09/2024.
//

#include "AssetManager.h"
#include "models/MapObject/WMap.h"
#include "models/LevelObject/OLibrary.h"
#include "models/LevelObject/Collectable.h"

#include <filesystem>
#include <iostream>

// Initialize the static member
AssetManager *AssetManager::instance = nullptr;

// Define the constructor
AssetManager::AssetManager() { /* Nothing in the constructor */ }

// Define the Meyers' Singleton instance method
AssetManager& AssetManager::GetInstance() {
        if (!instance) {
                instance = new AssetManager();
        }
        return *instance;
}

// Define the global reference
AssetManager& assetManager = AssetManager::GetInstance();

// Loads a library to memory without the image data
// I am creating the libraries as unique_ptrs to ensure they are deleted when the program ends ;)
// This also prevents them from going out of scope when the function ends
void AssetManager::LoadTextureLibrary(std::string fileName) {
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

        fileName = fileName.substr(fileName.find_last_of("/\\") + 1); // Get the file name
        fileName = fileName.substr(0, fileName.find_last_of('.')); // Drop the extension

        // Add the library to the map
        TextureLibraries[fileName] = std::move(library); // std::move to transfer ownership
        std::cout << "Loaded library: " << fileName << std::endl;
}
void AssetManager::LoadFonts(const std::string &directoryPath) {
        for (const auto &entry: std::filesystem::directory_iterator(directoryPath)) {
                if (entry.is_regular_file() && entry.path().extension() == ".ttf") {
                        std::string filePath = entry.path().string();
                        std::string fileNameWithoutExtension = entry.path().stem().string();

                        auto font = std::make_unique<sf::Font>();
                        if (font->loadFromFile(filePath)) {
                                assetManager.Fonts[fileNameWithoutExtension] = std::move(font);
                                std::cout << "Loaded font: " << fileNameWithoutExtension << std::endl;
                        } else {
                                std::cerr << "Failed to load font: " << filePath << std::endl;
                        }
                }
        }
}
void AssetManager::LoadMusic(const std::string& directoryPath) {
        for (const auto &entry: std::filesystem::directory_iterator(directoryPath)) {
                if (entry.is_regular_file() && (entry.path().extension() == ".mp3" ||
                                                entry.path().extension() == ".ogg" ||
                                                entry.path().extension() == ".wav")) {
                        std::string filePath = entry.path().string();
                        std::string fileNameWithoutExtension = entry.path().stem().string();

                        auto music = std::make_unique<sf::Music>();
                        if (music->openFromFile(filePath)) {
                                Music[fileNameWithoutExtension] = std::move(music);
                                std::cout << "Loaded music: " << fileNameWithoutExtension << std::endl;
                        } else {
                                std::cerr << "Failed to load music: " << filePath << std::endl;
                        }
                }
        }
}
void AssetManager::PlayMusic(const std::string& key, bool loop) {
        auto song = Music.find(key);
        if (song != Music.end()) {
                song->second->setLoop(loop);
                song->second->play();
        }
}
void AssetManager::StopMusic(const std::string& key) {
        auto song = Music.find(key);
        if (song != Music.end()) {
                song->second->stop();
        }
}
void AssetManager::SetMusicVolume(const std::string& key, float volume) {
        auto song = Music.find(key);
        if (song != Music.end()) {
                song->second->setVolume(volume); // 0-100
        }
}
