//
// Created by Paul McGinley on 13/05/2025.
//

#include "LoadingScene.h"

#include <SFML/Graphics/Text.hpp>
#include <unordered_set>

#include "GameScene.h"
#include "models/LevelObject/OLibrary.h"
#include "os/GetExecutionDirectory.h"
LoadingScene::LoadingScene() {}
LoadingScene::~LoadingScene() {}
void LoadingScene::BuildAssetQueue(const std::string& mapName) {
        WMap* map = asset_manager.Maps[mapName].get();


        // AssetQueue.emplace("sky", std::vector<int>{map->ParallaxBackgroundIndex});
        // AssetQueue.emplace("mountains", std::vector<int>{map->ParallaxBackgroundIndex});

        std::unordered_map<std::string, std::unordered_set<int>> libraryToIndices;

        for (const auto &wmObject: map->LevelObjects) {
                const auto &oLibraryName = wmObject.ObjectLibraryFile;

                // Skip if library doesn't exist
                if (!asset_manager.ObjectLibraries.contains(oLibraryName)) {
                        std::cerr << "Object library " << oLibraryName << " does not exist." << std::endl;
                        continue;
                }

                const auto &oLibrary = asset_manager.ObjectLibraries.at(oLibraryName);

                for (const auto &graphic: oLibrary->Images) {
                        // Skip graphics without an associated library
                        if (graphic.BackImageLibrary.empty()) {
                                std::cerr << "Graphic in library " << oLibraryName
                                          << " has no associated BackImageLibrary." << std::endl;
                                continue;
                        }

                        if (graphic.BackIndex < 0 ||
                            graphic.BackIndex >= asset_manager.TextureLibraries[graphic.BackImageLibrary]->entryCount) {
                                std::cerr << "Graphic in library " << oLibraryName << " has invalid BackIndex."
                                          << std::endl;
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

        for (auto &[library, indicesSet]: libraryToIndices) {
                std::vector<int> indices(indicesSet.begin(), indicesSet.end());
                std::sort(indices.begin(), indices.end()); // for consistent order

                for (size_t i = 0; i < indices.size(); i += loadPerFrame) {
                        size_t end = std::min(i + loadPerFrame, indices.size());
                        std::vector<int> batch(indices.begin() + i, indices.begin() + end);
                        AssetQueue.emplace(LibIndex{library, std::move(batch)});
                }
        }

        TargetValue = AssetQueue.size();
}
void LoadingScene::Update(const GameTime gameTime) {

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
        // Check if the loading is complete
        if (AssetQueue.empty()) {
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

        if (!AssetQueue.empty()) {
                const auto &currentLib = AssetQueue.front();
                std::string loadingTextStr = "Loading " + currentLib.library;
                sf::Text loadingText(loadingTextStr, font, 30);
                loadingText.setFillColor(sf::Color::White);
                loadingText.setPosition(window.getSize().x / 2 - loadingText.getGlobalBounds().width / 2,
                                        window.getSize().y  - 100);
                window.draw(loadingText);
        }


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
void LoadingScene::LateUpdate(GameTime) {}
void LoadingScene::DestroyScene() {}
void LoadingScene::OnScene_Active() {
        // Unloack framerate for faster loading
        game_manager.window->setFramerateLimit(0);

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

        //Frame needs to be centre and 300px from the bottom
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


        // progress bar needs to be 10px from the bottom and 10px from the left
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
void LoadingScene::OnScene_Deactivate() {}
