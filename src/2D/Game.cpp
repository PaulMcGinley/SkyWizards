//
// Created by Paul McGinley on 30/09/2024.
//

#include "Game.h"

#include <SFML/Graphics.hpp>

#include "managers/SceneManager.cpp"
#include "scenes/DevScene.h"
#include "scenes/SplashScreen.h"

Game::Game() {

}

void Game::Run() {

    // Add scenes to the scene manager
    sceneManager.addScene(SceneType::Splash, std::make_shared<SplashScreen>());
    sceneManager.addScene(SceneType::Dev, std::make_shared<DevScene>());

    sceneManager.changeScene(SceneType::Splash);


    // Set the window properties
    int screenWidth = 1920;
    int screenHeight = 1080;
    std::string title = "Legend of Sky Wizards - But from the side!";

    sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), title, sf::Style::Resize | sf::Style::Close);

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