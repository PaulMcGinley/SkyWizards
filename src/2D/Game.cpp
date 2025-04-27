//
// Created by Paul McGinley on 30/09/2024.
//

#include "Game.h"

#include <SFML/Graphics.hpp>

#include "managers/InputManager.cpp"
#include "managers/SceneManager.h"
#include "scenes/DevScene.h"
#include "scenes/GameScene.h"
#include "scenes/MainMenu.h"
#include "scenes/SplashScreen.h"

void Game::Run() {
        // Create a reference to the game manager for easy access

        // All scenes are managed by the scene manager and are added here
        scene_manager.AddScene(SceneType::SCENE_SPLASH, std::make_shared<SplashScreen>());
        scene_manager.AddScene(SceneType::SCENE_DEV, std::make_shared<DevScene>());
        scene_manager.AddScene(SceneType::SCENE_MAIN_MENU, std::make_shared<MainMenu>());
        scene_manager.AddScene(SceneType::SCENE_GAME, std::make_shared<GameScene>());

        // Set the current scene
        // This should always be the splash screen as this is scene it's purely for loading purposes
        // The assets for this scene are kept as external files to prevent decompression delays
        scene_manager.ChangeScene(SceneType::SCENE_SPLASH);


        // Main game loop
        sf::Event windowEvent { };
        while (game_manager.window->isOpen()) {
                while (game_manager.window->pollEvent(windowEvent)) {
                        // Close window: exit
                        if (windowEvent.type == sf::Event::Closed)
                                game_manager.window->close();
                }

                InputManager::getInstance().update();

                game_time += (clock.restart().asSeconds());
                scene_manager.Update(game_time);
                game_manager.window->clear(sf::Color(255,255,255,255));
                scene_manager.Draw(*game_manager.window, game_time);
                game_manager.window->display();
                scene_manager.LateUpdate(game_time);
        }
}
