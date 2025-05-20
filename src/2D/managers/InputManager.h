//
// Created by Paul McGinley on 20/05/2025.
//

#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <SFML/Window/Keyboard.hpp>
#include <unordered_map>

class InputManager {
public:
        // Method to get the instance of the singleton
        static InputManager& getInstance();

        // Delete copy constructor and assignment operator to prevent copies
        InputManager(const InputManager&) = delete;
        void operator=(const InputManager&) = delete;

        // Update the input device states
        void Update();

        // Check if a key is currently down
        [[nodiscard]] bool IsKeyDown(sf::Keyboard::Key key) const;

        // Check if a key is currently up
        [[nodiscard]] bool IsKeyUp(sf::Keyboard::Key key) const;

        // Check if a key was pressed (down now, up in the previous state)
        [[nodiscard]] bool IsKeyPressed(sf::Keyboard::Key key) const;

        // Check if a key was released (up now, but down in the previous state)
        [[nodiscard]] bool IsKeyReleased(sf::Keyboard::Key key) const;

private:
        // Private constructor to prevent instancing
        InputManager();

        std::unordered_map<int, bool> currentKeyboardState;
        std::unordered_map<int, bool> previousKeyboardState;
};

#endif //INPUTMANAGER_H
