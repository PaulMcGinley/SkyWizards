//
// Created by Paul McGinley on 30/09/2024.
//

#ifndef GAME_H
#define GAME_H

#include "models/GameTime.h"
#include "managers/SceneManager.cpp"
#include "managers/GameManager.cpp"

class Game {
public:
        //  Default constructor
        Game() = default;

        // Start the game loop
        void Run();
private:
        // Reference to the manager singletons for easy access
        SceneManager & scene_manager = SceneManager::getInstance();
        GameManager & game_manager = GameManager::getInstance();

        // Clock and game time using for tracking the game loop time
        sf::Clock clock;        // Clock tracks delta time and feeds the data to gameTime
        GameTime game_time;     // game_time tracks all time related data for the game
};

#endif //GAME_H
