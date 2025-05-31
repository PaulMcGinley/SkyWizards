//
// Created by Paul McGinley on 30/09/2024.
//

#include "Game.h"

#include <SFML/Graphics.hpp>
#include <filesystem>
#include "managers/InputManager.h"
#include "managers/SceneManager.h"
#include "scenes/CreditsScene.h"
#include "scenes/GameScene.h"
#include "scenes/LeaderboardScene.h"
#include "scenes/LoadingScene.h"
#include "scenes/MainMenu.h"
#include "scenes/Overlays/DebugOverlay.h"
#include "scenes/Overlays/EndOfLevel.h"
#include "scenes/Overlays/SubmitScore.h"
#include "scenes/SettingsScene.h"
#include "scenes/SplashScreen.h"

void Game::Run() {
        // All scenes are managed by the scene manager and are added here
        sceneManager.AddScene(SceneType::SCENE_SPLASH, std::make_shared<SplashScreen>());
        sceneManager.AddScene(SceneType::SCENE_MAIN_MENU, std::make_shared<MainMenu>());
        sceneManager.AddScene(SceneType::SCENE_GAME, std::make_shared<GameScene>());
        sceneManager.AddScene(SceneType::SCENE_LOADER, std::make_shared<LoadingScene>());
        sceneManager.AddScene(SceneType::SCENE_OPTIONS, std::make_shared<SettingsScene>());
        sceneManager.AddScene(SceneType::SCENE_CREDITS, std::make_shared<CreditsScene>());
        sceneManager.AddScene(SceneType::SCENE_LEADERBOARD, std::make_shared<LeaderboardScene>());

        // Overlays
        sceneManager.AddScene(SceneType::SCENE_DEBUG_OVERLAY, std::make_shared<DebugOverlay>());
        sceneManager.AddScene(SceneType::SCENE_END_OF_LEVEL, std::make_shared<EndOfLevel>());
        sceneManager.AddScene(SceneType::SCENE_SUBMIT_SCORE, std::make_shared<SubmitScore>());

        // Set the current scene
        // This should always be the splash screen as this is scene it's purely for loading purposes
        // The assets for this scene are kept as external files to prevent decompression delays
        sceneManager.ChangeScene(SceneType::SCENE_SPLASH);

        const std::shared_ptr<DebugOverlay> debugOverlay = std::dynamic_pointer_cast<DebugOverlay>(sceneManager.GetScene(SceneType::SCENE_DEBUG_OVERLAY));

        // Main game loop
        sf::Event windowEvent{};
        while (gameManager.window->isOpen()) {
                while (gameManager.window->pollEvent(windowEvent)) {
                        // Close window: exit
                        if (windowEvent.type == sf::Event::Closed)
                                gameManager.window->close();
                }

                InputManager::GetInstance().Update();

                if (input.IsKeyDown(sf::Keyboard::Num1) && input.IsKeyDown(sf::Keyboard::Num2)) {
                        gameManager.window->close();
                        // TODO: Change this to an Exit to main menu system
                }

                if (input.ShowDebugPressed()) {
                        gameManager.ToggleDebug();
                }

                if (input.ShowCollisionBoxsPressed()) {
                        gameManager.ToggleShowCollisions();
                }

                gameTime += (clock.restart().asSeconds());
                sceneManager.Update(gameTime);
                gameManager.window->clear(sf::Color(255, 255, 255, 255));
                sceneManager.Draw(*gameManager.window, gameTime);

                if (gameManager.Debug()) {
                        debugOverlay->Update(gameTime);
                        debugOverlay->Draw(*gameManager.window, gameTime);
                }

                gameManager.window->display();
                sceneManager.LateUpdate(gameTime);
        }
}
