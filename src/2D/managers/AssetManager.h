//
// Created by Paul McGinley on 23/04/2025.
//

#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include <map>
#include <vector>
#include "models/TextureLibrary.h"
#include <SFML/Graphics/Font.hpp>
#include <SFML/Audio/Music.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Sound.hpp>
#include <memory>

// Forward declarations to pointers
class WMap;
class OLibrary;
class Collectable;

class AssetManager {
public:
        // Delete copy constructor and assignment
        AssetManager(const AssetManager&) = delete;
        AssetManager& operator=(const AssetManager&) = delete;

        // Static method to get instance
        static AssetManager& GetInstance();

        // Holds all the data from the *.lib files
        std::map<std::string, std::unique_ptr<TextureLibrary>> TextureLibraries;

        // Holds all the data from the *.olx files
        std::map<std::string, std::unique_ptr<OLibrary>> ObjectLibraries;

        // Holds all the data from the *.wmap files
        std::map<std::string, std::unique_ptr<WMap>> Maps;

        // Holds all the fonts (*.ttf files)
        std::map<std::string, std::unique_ptr<sf::Font>> Fonts;

        // Holds all music files
        std::map<std::string, std::unique_ptr<sf::Music>> Music;

        // Holds all Collectable objects
        std::map<std::string, std::vector<std::unique_ptr<Collectable>>> Collectables;

        // Holds all sound buffers
        std::map<std::string, std::unique_ptr<sf::SoundBuffer>> SoundBuffers;

        // Store active sounds
        std::vector<std::unique_ptr<sf::Sound>> ActiveSounds;

        void LoadTextureLibrary(std::string fileName);
        static void LoadFonts(const std::string &directoryPath);
        void LoadMusic(const std::string& directoryPath);
        void PlayMusic(const std::string& key, bool loop = true);
        void StopMusic(const std::string& key);
        void SetMusicVolume(float volume);
        void LoadSoundEffects(const std::string& directoryPath);
        void PlaySoundEffect(const std::string& key, float volume, float pitch);
        void CleanupFinishedSounds();

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
extern AssetManager& assetManager;

#endif //ASSETMANAGER_H
