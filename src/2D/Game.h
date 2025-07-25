//
// Created by Paul McGinley on 30/09/2024.
//

#ifndef GAME_H
#define GAME_H

#include "managers/GameManager.h"
#include "managers/SceneManager.h"
#include "models/GameTime.h"
#include "scenes/Overlays/DebugOverlay.h"

class Game {
public:
        //  Default constructor
        Game() = default;

        // Start the game loop
        void Run();
private:
        // Reference to the manager singletons for easy access
        SceneManager & sceneManager = SceneManager::GetInstance();
        GameManager & gameManager = GameManager::GetInstance();
        InputManager & input = InputManager::GetInstance();

        // Clock and game time using for tracking the game loop time
        sf::Clock clock;        // Clock tracks time and feeds the into to gameTime
        GameTime gameTime;      // gameTime tracks all time related data for the game
};

#endif //GAME_H
