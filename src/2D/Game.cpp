//
// Created by Paul McGinley on 30/09/2024.
//

#include "Game.h"

#include <SFML/Graphics.hpp>

#include "managers/SceneManager.cpp"
#include "scenes/DevScene.h"
#include "scenes/MainMenu.h"
#include "scenes/SplashScreen.h"

Game::Game() {
    // TODO: Load settings
    // TODO: Load assets
    // TODO: Load save data
}

void Game::Run() {
    // Set the window properties
    int screenWidth = 1920;
    int screenHeight = 1080;
    std::string title = "Legend of Sky Wizards - But from the side!";
    int style = sf::Style::Close | sf::Style::Titlebar;
    sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), title, style);


    // Add scenes to the scene manager
    sceneManager.addScene(SceneType::Splash, std::make_shared<SplashScreen>(), window );
    sceneManager.addScene(SceneType::Dev, std::make_shared<DevScene>(), window);
    sceneManager.addScene(SceneType::FirstMenu, std::make_shared<MainMenu>(), window);
    sceneManager.changeScene(SceneType::Splash);


    // Main game loop
    sf::Event windowEvent { };
    while (window.isOpen()) {

        while (window.pollEvent(windowEvent)) {
            // Close window: exit
            if (windowEvent.type == sf::Event::Closed)
                window.close();
        }
        gameTime += (clock.restart().asSeconds());

        sceneManager.Update(gameTime);
        window.clear(sf::Color(255,255,255,255));
        sceneManager.Draw(window, gameTime);
        window.display();
        sceneManager.LateUpdate(gameTime);
    }
}