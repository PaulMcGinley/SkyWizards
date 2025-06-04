//
// Created by Paul McGinley on 30/09/2024.
//

#include "AssetManager.h"
#include "models/MapObject/WMap.h"
#include "models/LevelObject/OLibrary.h"
#include "models/LevelObject/Collectable.h"

#include <filesystem>
#include <iostream>

#include "GameManager.h"

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
        SetMusicVolume(GameManager::GetInstance().GetMusicVolume());
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
void AssetManager::SetMusicVolume(float volume) {
        for (auto& [_, song] : Music) {
                song->setVolume(volume);
        }
}
void AssetManager::LoadSoundEffects(const std::string& directoryPath) {
        // Get the base directory to extract relative paths
        std::filesystem::path baseDir = std::filesystem::path(directoryPath).parent_path();
        std::string baseDirStr = baseDir.string();

        for (const auto &entry: std::filesystem::recursive_directory_iterator(directoryPath)) {
                if (entry.is_regular_file() && (entry.path().extension() == ".wav" ||
                                                entry.path().extension() == ".ogg" ||
                                                entry.path().extension() == ".mp3")) {
                        std::string filePath = entry.path().string();

                        // Extract relative path without the extension
                        std::string relativePath = filePath.substr(baseDirStr.length() + 1);
                        std::filesystem::path relPath(relativePath);
                        std::string keyPath = (relPath.parent_path() / relPath.stem()).string();

                        // Replace backslashes with forward slashes for consistency
                        std::replace(keyPath.begin(), keyPath.end(), '\\', '/');

                        auto soundBuffer = std::make_unique<sf::SoundBuffer>();
                        if (soundBuffer->loadFromFile(filePath)) {
                                SoundBuffers[keyPath] = std::move(soundBuffer);
                                std::cout << "Loaded sound effect with key: " << keyPath << std::endl;
                        } else {
                                std::cerr << "Failed to load sound effect: " << filePath << std::endl;
                        }
                }
        }
}
void AssetManager::PlaySoundEffect(const std::string& key, float volume, float pitch) {
        // Clean up any finished sounds to free space
        CleanupFinishedSounds();
        auto buffer = SoundBuffers.find(key);
        if (buffer != SoundBuffers.end()) {
                // Create a new sound object and add it to our active sounds
                auto sound = std::make_unique<sf::Sound>();
                sound->setBuffer(*buffer->second);
                // volume depricated, useing GameManager instead
                sound->setVolume(GameManager::GetInstance().GetSFXVolume());
                sound->setPitch(pitch);
                sound->play();

                // Add to active sounds vector
                ActiveSounds.push_back(std::move(sound));
        }
}
void AssetManager::CleanupFinishedSounds() {
        // Remove sounds that have finished playing
        ActiveSounds.erase(std::remove_if(ActiveSounds.begin(), ActiveSounds.end(),
                [](const std::unique_ptr<sf::Sound>& sound) {
                        return sound->getStatus() == sf::Sound::Stopped;
                }),
            ActiveSounds.end()
        );
}
