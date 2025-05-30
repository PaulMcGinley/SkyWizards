//
// Created by Paul McGinley on 25/05/2025.
//

#include "EndOfLevel.h"

#include "managers/InputManager.h"
#include "scenes/LoadingScene.h"

void EndOfLevel::Update(GameTime gameTime) {
        InputManager &input = InputManager::GetInstance();
        if (input.IsConfirmPressed()) {
                std::vector<std::string> maps = {"Lv_01", "Lv_02", "00"};
                auto it = std::find(maps.begin(), maps.end(), mapName);

                // Check if current level is the last one
                if (it != maps.end() && (it == maps.end() - 1)) {
                        // Last level completed, return to main menu
                        assetManager.StopMusic(assetManager.Maps[mapName]->song);
                        sceneManager.ChangeScene(SceneType::SCENE_MAIN_MENU);
                } else {
                        // Not the last level, proceed to next level
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
}
void EndOfLevel::LateUpdate(GameTime gameTime) {}
void EndOfLevel::Draw(sf::RenderWindow &window, GameTime gameTime) {
        window.draw(shade); // Draw the share to bring focus to the overlay

        int screenWidth = gameManager.getResolutionWidth();
        int screenHeight = gameManager.getResolutionHeight();

        IDraw::DrawText(window, "OpenSans-ExtraBold", "LEVEL COMPLETE!", sf::Vector2f(screenWidth/2,250), Align::CENTER, 50, sf::Color::White, 20);
        IDraw::DrawText(window, "OpenSans-ExtraBold", "Press CONFIRM to continue ...", sf::Vector2f(screenWidth/2,screenHeight-250), Align::CENTER, 25, sf::Color::White, 20);

        window.draw(rating);
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
        mobsAvailable = 0;
        mobsKilled = 0;
        coinsAvailable = 0;
        coinsCollected = 0;
        timeTaken = 0;
}
void EndOfLevel::SetMobData(int available, int killed) {
        mobsAvailable = available;
        mobsKilled = killed;
}
void EndOfLevel::SetCoinData(int available, int collected) {
        coinsAvailable = available;
        coinsCollected = collected;
}
void EndOfLevel::SetTimeTaken(int timeInMilliseconds) { timeTaken = timeInMilliseconds; }
void EndOfLevel::CalculatePercentComplete() {
        bool coinComplete = false;
        bool mobsComplete = false;
        bool timeComplete = false;

        coinComplete = coinsAvailable - coinsCollected <= 0;
        mobsComplete = mobsAvailable - mobsKilled <= 0;
        timeComplete = timeTaken <= 45; // 45 seconds in milliseconds

        int starIndex = 43 + coinComplete + mobsComplete + timeComplete;

        rating.setTexture( assetManager.TextureLibraries["PrgUse"] ->entries[starIndex].texture, true);
        rating.setOrigin(rating.getTexture()->getSize().x / 2, rating.getTexture()->getSize().y / 2);
        rating.setPosition(gameManager.getResolutionWidth() / 2, gameManager.getResolutionHeight() / 2);

}
void EndOfLevel::SetMapName(const std::string &string) {
        mapName = string;
}
