//
// Created by Paul McGinley on 24/04/2025.
//

#include "GameScene.h"

#include <iostream>
#include <unordered_map>
#include <unordered_set>

#include "managers/SceneManager.h"
#include "models/LevelObject/OLibrary.h"
#include "models/MapObject/WMap.h"

GameScene::GameScene()
        : IScene()
        , map(nullptr) {

}

void GameScene::Update(GameTime gameTime) {

}

void GameScene::LateUpdate(GameTime gameTime) {

}

void GameScene::Draw(sf::RenderWindow &window, GameTime gameTime) {
        if (!IsInitialized()) {
                // Draw loading screen
                return;
        }

        // Set the view
        //window.setView(viewport);

        // Draw game

        // Draw the parallax background
        window.draw(skyBoxSprite);
        window.draw(mountainsSprite);
}

void GameScene::InitializeScene() {
       // LoadAssets();

        IScene::InitializeScene();
}

void GameScene::DestroyScene() {

}

void GameScene::OnScene_Active() {

}

void GameScene::OnScene_Deactivate() {

}

void GameScene::LoadAssets() {
        // Check if the map exists in the asset manager
        if (asset_manager.Maps.contains(mapName)) {
                // Get the map from the asset manager
                map = asset_manager.Maps[mapName].get();
        } else {
                std::cerr << "Map " << mapName << " does not exist in the asset manager." << std::endl;
                SceneManager::GetInstance().ChangeScene(SceneType::SCENE_MAIN_MENU);
                return;
        }

        // Check if the ParallaxBackgroundIndex is valid
        if (map->ParallaxBackgroundIndex < 0 || map->ParallaxBackgroundIndex >= asset_manager.TextureLibraries["sky"]->entryCount)
                map->ParallaxBackgroundIndex = 0; // Default to 0 if invalid

        // Load the parallax background texture
        asset_manager.TextureLibraries["sky"]->LoadIndices({map->ParallaxBackgroundIndex});
        skyBoxTexture = &asset_manager.TextureLibraries["sky"]->entries[map->ParallaxBackgroundIndex].texture;
        skyBoxSprite.setTexture(*skyBoxTexture, true);
        skyBoxSprite.setPosition(0, 0); // Set the position of the skyBoxSprite to the top left corner

        // Check if the mountains background index is valid
        if (map->MountainsBackgroundIndex < 0 || map->MountainsBackgroundIndex >= asset_manager.TextureLibraries["mountains"]->entryCount)
                map->MountainsBackgroundIndex = 0; // Default to 0 if invalid

        // Load the mountains background texture
        asset_manager.TextureLibraries["mountains"]->LoadIndices({map->MountainsBackgroundIndex});
        mountainsTexture = &asset_manager.TextureLibraries["mountains"]->entries[0].texture;
        mountainsSprite.setTexture(*mountainsTexture, true);


        // Load required assets
        // Map from texture library name to set of unique indices
        std::unordered_map<std::string, std::unordered_set<int>> libraryToIndices;

        for (const auto& wmObject : map->LevelObjects) {
                const auto& objectLibrary = wmObject.ObjectLibraryFile;

                // Skip if library doesn't exist
                if (!asset_manager.ObjectLibraries.contains(objectLibrary)) {
                        std::cerr << "Object library " << objectLibrary << " does not exist." << std::endl;
                        continue;
                }

                const auto& oLibrary = asset_manager.ObjectLibraries.at(objectLibrary);

                for (const auto& graphic : oLibrary->Images) {
                        // Skip graphics without an associated library
                        if (graphic.BackImageLibrary.empty()) {
                                std::cerr << "Graphic in library " << objectLibrary << " has no associated BackImageLibrary." << std::endl;
                                continue;
                        }

                        if ( graphic.BackIndex < 0 || graphic.BackIndex >= oLibrary->Images.size()) {
                                std::cerr << "Graphic in library " << objectLibrary << " has invalid BackIndex." << std::endl;
                                std::cerr << "Expected range: [0, " << oLibrary->Images.size() << "]" << std::endl;
                                std::cerr << "Actual BackIndex: " << graphic.BackIndex << std::endl;
                                continue;
                        }

                        if (graphic.BackEndIndex == -1) {
                                // If BackEndIndex is -1, only add BackIndex
                                libraryToIndices[graphic.BackImageLibrary].insert(graphic.BackIndex);
                        } else {
                                // Otherwise add the range from BackIndex to BackEndIndex
                                for (int idx = graphic.BackIndex; idx <= graphic.BackEndIndex; ++idx) {
                                        libraryToIndices[graphic.BackImageLibrary].insert(idx);
                                }
                        }
                }
        }

        // Load the indices for each texture library
        for (const auto& [libraryName, indicesSet] : libraryToIndices) {
                if (asset_manager.TextureLibraries.contains(libraryName)) {
                        std::vector<int> indices(indicesSet.begin(), indicesSet.end());
                        asset_manager.TextureLibraries[libraryName]->LoadIndices(indices);
                }
        }
}