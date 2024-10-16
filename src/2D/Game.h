//
// Created by Paul McGinley on 30/09/2024.
//

#ifndef GAME_H
#define GAME_H

#include "models/GameTime.h"
#include "managers/SceneManager.cpp"

class Game {
public:
    // Constructor
    Game();

    // Start the game loop
    void Run();
private:
    SceneManager& sceneManager = SceneManager::getInstance();
    sf::Clock clock;
    GameTime gameTime;
};

#endif //GAME_H
