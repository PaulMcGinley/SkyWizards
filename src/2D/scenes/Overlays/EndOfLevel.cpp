//
// Created by Paul McGinley on 25/05/2025.
//

#include "EndOfLevel.h"

#include "managers/InputManager.h"
#include "scenes/LoadingScene.h"
void EndOfLevel::Update(GameTime gameTime) {
        InputManager &input = InputManager::getInstance();
        if (input.IsKeyPressed(sf::Keyboard::Space)) {
                std::vector<std::string> maps = {"Lv_01", "Lv_02"};
                auto it = std::find(maps.begin(), maps.end(), mapName);
                size_t nextIndex = 0;
                if (it != maps.end()) {
                        nextIndex = (std::distance(maps.begin(), it) + 1) % maps.size();
                }
                std::string nextMap = maps[nextIndex];

                auto scenePtr = sceneManager.GetScene(SceneType::SCENE_LOADER);
                auto gameScene = std::dynamic_pointer_cast<LoadingScene>(scenePtr);
                if (gameScene) {
                        gameScene->BuildAssetQueue(nextMap);
                }

                assetManager.StopMusic(assetManager.Maps[mapName]->song);
                sceneManager.ChangeScene(SceneType::SCENE_LOADER);
        }
}
void EndOfLevel::LateUpdate(GameTime gameTime) {}
void EndOfLevel::Draw(sf::RenderWindow &window, GameTime gameTime) {
        window.draw(shade); // Draw the share to bring focus to the overlay

        IDraw::DrawText(window, "OpenSans-ExtraBold", "LEVEL COMPLETE!", sf::Vector2f(1920/2,250), Align::CENTER, 50, sf::Color::White, 20);
}
void EndOfLevel::InitializeScene() {
        sf::Vector2i screenSize ={static_cast<int>(gameManager.getResolutionWidth()), static_cast<int>(gameManager.getResolutionHeight())};
        shade.setSize(sf::Vector2f(screenSize.x, screenSize.y));
        shade.setFillColor(sf::Color(0, 0, 0, 150)); // Semi-transparent black shade

        IScene::InitializeScene();
}
void EndOfLevel::DestroyScene() {}
void EndOfLevel::OnScene_Activate() {}
void EndOfLevel::OnScene_Deactivate() {}
void EndOfLevel::ResetBoard() {
        slimesAvailable = 0;
        slimesKilled = 0;
        chestMonstersAvailable = 0;
        chestMonstersKilled = 0;
        angryMushroomsAvailable = 0;
        angryMushroomsKilled = 0;
        cactusMonstersAvailable = 0;
        cactusMonstersKilled = 0;
        coinsAvailable = 0;
        coinsCollected = 0;
        timeTaken = 0;
}
void EndOfLevel::SetSlimeData(int available, int killed) {}
void EndOfLevel::SetChestMonsterData(int available, int killed) {}
void EndOfLevel::SetAngryMushroomData(int available, int killed) {}
void EndOfLevel::SetCactusMonsterData(int available, int killed) {}
void EndOfLevel::SetCoinData(int available, int collected) {}
void EndOfLevel::SetTimeTaken(int timeInMilliseconds) { timeTaken = timeInMilliseconds; }
void EndOfLevel::CalculatePercentComplete() {
        float slimePercent = (slimesAvailable > 0) ? (static_cast<float>(slimesKilled) / slimesAvailable) * 100.f : 0.f;
        float chestMonsterPercent =
                        (chestMonstersAvailable > 0)
                                        ? (static_cast<float>(chestMonstersKilled) / chestMonstersAvailable) * 100.f
                                        : 0.f;
        float angryMushroomPercent =
                        (angryMushroomsAvailable > 0)
                                        ? (static_cast<float>(angryMushroomsKilled) / angryMushroomsAvailable) * 100.f
                                        : 0.f;
        float cactusMonsterPercent =
                        (cactusMonstersAvailable > 0)
                                        ? (static_cast<float>(cactusMonstersKilled) / cactusMonstersAvailable) * 100.f
                                        : 0.f;
        float coinPercent = (coinsAvailable > 0) ? (static_cast<float>(coinsCollected) / coinsAvailable) * 100.f : 0.f;

        percentComplete = (slimePercent + chestMonsterPercent + angryMushroomPercent + cactusMonsterPercent +
                           coinPercent) /
                          5.f;
}
void EndOfLevel::SetMapName(const std::string &string) {
        mapName = string;
}
