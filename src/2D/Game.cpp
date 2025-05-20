//
// Created by Paul McGinley on 30/09/2024.
//

#include "Game.h"

#include <SFML/Graphics.hpp>
#include <filesystem>
#include "managers/InputManager.cpp"
#include "managers/SceneManager.h"
#include "scenes/DevScene.h"
#include "scenes/GameScene.h"
#include "scenes/LoadingScene.h"
#include "scenes/MainMenu.h"
#include "scenes/Overlays/DebugOverlay.h"
#include "scenes/SettingsScene.h"
#include "scenes/SplashScreen.h"

void Game::Run() {
        game_manager.window->setVerticalSyncEnabled(true);

        // All scenes are managed by the scene manager and are added here
        scene_manager.AddScene(SceneType::SCENE_SPLASH, std::make_shared<SplashScreen>());
        scene_manager.AddScene(SceneType::SCENE_DEV, std::make_shared<DevScene>());
        scene_manager.AddScene(SceneType::SCENE_MAIN_MENU, std::make_shared<MainMenu>());
        scene_manager.AddScene(SceneType::SCENE_GAME, std::make_shared<GameScene>());
        scene_manager.AddScene(SceneType::SCENE_DEBUG_OVERLAY, std::make_shared<DebugOverlay>());
        scene_manager.AddScene(SceneType::SCENE_LOADER, std::make_shared<LoadingScene>());
        scene_manager.AddScene(SceneType::SCENE_OPTIONS, std::make_shared<SettingsScene>());

        // Set the current scene
        // This should always be the splash screen as this is scene it's purely for loading purposes
        // The assets for this scene are kept as external files to prevent decompression delays
        scene_manager.ChangeScene(SceneType::SCENE_SPLASH);

        std::shared_ptr<DebugOverlay> debugOverlay = std::dynamic_pointer_cast<DebugOverlay>(scene_manager.GetScene(SceneType::SCENE_DEBUG_OVERLAY));

        // Main game loop
        sf::Event windowEvent{};
        while (game_manager.window->isOpen()) {
                while (game_manager.window->pollEvent(windowEvent)) {
                        // Close window: exit
                        if (windowEvent.type == sf::Event::Closed)
                                game_manager.window->close();
                }
                // Close window by key input
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1) && sf::Keyboard::isKeyPressed(sf::Keyboard::Num2) ||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::N) && sf::Keyboard::isKeyPressed(sf::Keyboard::M)) {
                        game_manager.window->close();
                }

                InputManager::getInstance().update();

                game_time += (clock.restart().asSeconds());
                scene_manager.Update(game_time);
                game_manager.window->clear(sf::Color(255, 255, 255, 255));
                scene_manager.Draw(*game_manager.window, game_time);

                debugOverlay->Update(game_time);
                debugOverlay->Draw(*game_manager.window, game_time);

                game_manager.window->display();
                scene_manager.LateUpdate(game_time);
        }
}