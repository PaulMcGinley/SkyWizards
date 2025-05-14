//
// Created by Paul McGinley on 13/05/2025.
//

#include "LoadingScene.h"

#include <SFML/Graphics/Text.hpp>
#include <unordered_set>
#include <set>

#include "GameScene.h"
#include "models/LevelObject/OLibrary.h"
#include "os/GetExecutionDirectory.h"

LoadingScene::LoadingScene() { /* Nothing in the constructor */ }
LoadingScene::~LoadingScene() { /* Nothing in the destructor */ }
void LoadingScene::BuildAssetQueue(const std::string& mapName) {
        WMap* previousMap = asset_manager.Maps[game_manager.GetLastPlayedMap()].get();
        WMap* nextMap = asset_manager.Maps[mapName].get();

        // Lambda function to get all the assets in a map
        // [&] Captures all variables from the surrounding scope (BuildAssetQueue) by reference
        auto GetMapAssets = [&](WMap* map) {
                std::set<std::pair<std::string, int>> assets;

                // Check if the map is valid
                if (!map) {
                        std::cerr << "GetMapAssets - Invalid map provided (nullptr)." << std::endl;
                        return assets;
                }

                // Parallax backgrounds
                int skyIndex = map->ParallaxBackgroundIndex;
                int mountainsIndex = map->MountainsBackgroundIndex;
                if (skyIndex >= 0) assets.emplace("sky", skyIndex);
                if (mountainsIndex >= 0) assets.emplace("mountains", mountainsIndex);

                // Loop through all the Level Objects
                for (const auto& wmObject : map->LevelObjects) {
                        // Get the Object Library name
                        const auto& oLibraryName = wmObject.ObjectLibraryFile;
                        // Check if the library exists
                        if (!asset_manager.ObjectLibraries.contains(oLibraryName))
                                continue; // Library does not exist

                        // Get the Object Library
                        const auto& oLibrary = asset_manager.ObjectLibraries.at(oLibraryName);
                        // Loop through all the images in the library
                        for (const auto& graphic : oLibrary->Images) {
                                // Check if Library name is present
                                if (graphic.BackImageLibrary.empty())
                                        continue; // No library name
                                // Check if the Image is not animated
                                if (graphic.BackEndIndex == -1) {
                                        assets.emplace(graphic.BackImageLibrary, graphic.BackIndex); // Add the single index
                                } else {
                                        // Load all indices in the range
                                        for (int idx = graphic.BackIndex; idx <= graphic.BackEndIndex; ++idx) {
                                                assets.emplace(graphic.BackImageLibrary, idx);
                                        }
                                }
                        }
                }
                return assets;
        };

        // Get the assets for the previous map
        auto previousMapAssets = GetMapAssets(previousMap);
        // Get the assets for the next map
        auto nextMapAssets = GetMapAssets(nextMap);

        // Unload assets not needed anymore
        for (const auto& asset : previousMapAssets) {
                // Check if the asset is not in the next map
                if (!nextMapAssets.count(asset)) {
                        // Get the Texture Library
                        auto& textureLibrary = asset_manager.TextureLibraries[asset.first];
                        if (textureLibrary) {
                                textureLibrary->UnloadIndices({asset.second}); // Unload the asset
                        }
                }
        }

        // Map to hold library names and their indices
        std::unordered_map<std::string, std::unordered_set<int>> libraryIndicesToLoad;
        // Loop through the next map assets
        for (const auto& asset : nextMapAssets) {
                // Check if the asset is not in the previous map
                if (!previousMapAssets.count(asset)) {
                        libraryIndicesToLoad[asset.first].insert(asset.second); // Add the asset to the map
                }
        }

        // Build the AssetQueue
        AssetQueue = {};
        // Loop through the library indices to load
        for (auto& [library, indicesSet] : libraryIndicesToLoad) {
                // Create a vector to hold indices of the library
                std::vector<int> indices(indicesSet.begin(), indicesSet.end());
                // Sort the indices to ensure they are in order
                std::sort(indices.begin(), indices.end());
                // Split the indices into batches
                for (size_t i = 0; i < indices.size(); i += ASSET_BATCH_SIZE) {
                        // Calculate the end index for the batch ensuring it doesn't exceed the size
                        size_t end = std::min(i + ASSET_BATCH_SIZE, indices.size());
                        // Create the batch of indices (up to, but not including, end)
                        std::vector<int> batch(indices.begin() + i, indices.begin() + end);
                        // Add the library and batch indices to the AssetQueue
                        AssetQueue.emplace(LibIndex{library, std::move(batch)});
                }
        }

        // Set the target value for progress tracking
        TargetValue = AssetQueue.size();

        // Set the Last Played Map name in the game manager for the next time we need to load assets
        game_manager.SetLastPlayedMap(mapName);
}
void LoadingScene::Update(const GameTime gameTime) {

        if (nextSceneTime ==0 && !AssetQueue.empty()) {
                // Load the next batch of assets
                const auto &currentLib = AssetQueue.front();
                auto &textureLibrary = asset_manager.TextureLibraries[currentLib.library];
                if (textureLibrary) {
                        // Load the indices for the current library
                        textureLibrary->LoadIndices(currentLib.indices);
                        AssetQueue.pop();
                } else {
                        std::cerr << "Texture library " << currentLib.library << " does not exist." << std::endl;
                        AssetQueue.pop();
                }
        }

        if (AssetQueue.empty() && nextSceneTime == 0) {
                nextSceneTime = gameTime.NowAddMilliseconds(1000);
        }

        // Check if the loading is complete
        if (AssetQueue.empty() && nextSceneTime > 0 && gameTime.TimeElapsed(nextSceneTime)) {
                auto scenePtr = scene_manager.GetScene(SceneType::SCENE_GAME);
                auto gameScene = std::dynamic_pointer_cast<GameScene>(scenePtr);
                if (gameScene) {
                        gameScene->LoadMap("Mob_Test");
                }

                // Loack framerate again
                game_manager.window->setFramerateLimit(60);
                scene_manager.ChangeScene(SceneType::SCENE_GAME);
        }
}
void LoadingScene::Draw(sf::RenderWindow &window, GameTime gameTime) {
        window.draw(backgroundQuad, &asset_manager.TextureLibraries["PrgUse"].get()->entries[0].texture);
        // Draw the loading text
        sf::Font font = *asset_manager.Fonts["OpenSans-Bold"].get();

        std::string loadingTextStr;
        if (!AssetQueue.empty()) {
                loadingTextStr = "Loading " + AssetQueue.front().library;
        } else {
                loadingTextStr = "Loading Complete";
        }
        sf::Text loadingText(loadingTextStr, font, 30);
        loadingText.setFillColor(sf::Color::White);
        loadingText.setPosition(window.getSize().x / 2 - loadingText.getGlobalBounds().width / 2,
                                window.getSize().y  - 100);
        window.draw(loadingText);



        CurrentValue = TargetValue - AssetQueue.size();
        // Calculate progress percentage
        float progressPercentage = static_cast<float>(CurrentValue) / static_cast<float>(TargetValue);

        // Frame sprite
        sf::Sprite frameSprite(frame);
        float frameX = (game_manager.getResolutionWidth() - frameSprite.getGlobalBounds().width) / 2;
        float frameY = game_manager.getResolutionHeight() * 0.75f;
        frameSprite.setPosition(frameX, frameY);


        // Progress bar
        sf::RectangleShape progressBar;
        float progressBarWidth = frameSprite.getGlobalBounds().width * 0.9f;
        float progressBarHeight = frameSprite.getGlobalBounds().height * 0.4f;

        float progressX = frameX + (frameSprite.getGlobalBounds().width - progressBarWidth) / 2;
        float progressY = frameY + (frameSprite.getGlobalBounds().height - progressBarHeight) / 2;

        progressBar.setSize(sf::Vector2f(progressBarWidth * progressPercentage, progressBarHeight));
        progressBar.setPosition(progressX, progressY);
        progressBar.setFillColor(sf::Color(50, 50, 180));

        window.draw(progressBar);
        window.draw(frameSprite);
}
void LoadingScene::LateUpdate(GameTime) { /* No late updates */}
void LoadingScene::DestroyScene() {/* Nothing to destroy */}
void LoadingScene::OnScene_Active() {
        // Unlock framerate for faster loading
        game_manager.window->setFramerateLimit(0);

        nextSceneTime = 0;

        std::string exeDir = getExecutableDirectory();
        frame.loadFromFile(exeDir + "/resources/loader/frame.png");
        progress.loadFromFile(exeDir + "/resources/loader/progressbar.png");

        int screenWidth = game_manager.getResolution().x;
        int screenHeight = game_manager.getResolution().y;

        backgroundQuad[0].position = sf::Vector2f(0, 0);
        backgroundQuad[1].position = sf::Vector2f(screenWidth, 0);
        backgroundQuad[2].position = sf::Vector2f(screenWidth, screenHeight);
        backgroundQuad[3].position = sf::Vector2f(0, screenHeight);

        backgroundQuad[0].texCoords = sf::Vector2f(0, 0);
        backgroundQuad[1].texCoords = sf::Vector2f(screenWidth, 0);
        backgroundQuad[2].texCoords = sf::Vector2f(screenWidth, screenHeight);
        backgroundQuad[3].texCoords = sf::Vector2f(0, screenHeight);

        // Progress bar frame
        float frameWidth = static_cast<float>(frame.getSize().x);
        float frameHeight = static_cast<float>(frame.getSize().y);

        sf::Vector2f frameStartPic = {518,850   };
        frameQuad[0].position = {frameStartPic.x, frameStartPic.y};
        frameQuad[1].position = {frameStartPic.x + frameWidth, frameStartPic.y};
        frameQuad[2].position = {frameStartPic.x + frameWidth, frameStartPic.y + frameHeight};
        frameQuad[3].position = {frameStartPic.x, frameStartPic.y + frameHeight};

        frameQuad[0].texCoords = {0, 0};
        frameQuad[1].texCoords = {frameWidth, 0};
        frameQuad[2].texCoords = {frameWidth, frameHeight};
        frameQuad[3].texCoords = {0, frameHeight};

        // Progress bar
        float progressWidth = static_cast<float>(progress.getSize().x);
        float progressHeight = static_cast<float>(progress.getSize().y);

        sf::Vector2f progressStartPic = {562,893};
        progressQuad[0].position = {progressStartPic.x, progressStartPic.y};
        progressQuad[1].position = {progressStartPic.x + progressWidth, progressStartPic.y};
        progressQuad[2].position = {progressStartPic.x + progressWidth, progressStartPic.y + progressHeight};
        progressQuad[3].position = {progressStartPic.x, progressStartPic.y + progressHeight};

        progressQuad[0].texCoords = {0, 0};
        progressQuad[1].texCoords = {static_cast<float>(progress.getSize().x), 0};
        progressQuad[2].texCoords = {static_cast<float>(progress.getSize().x), static_cast<float>(progress.getSize().y)};
        progressQuad[3].texCoords = {0, static_cast<float>(progress.getSize().y)};
}
void LoadingScene::OnScene_Deactivate() { /* Nothing to deactivate */ }
