//
// Created by Paul McGinley on 09/12/2024.
//

#include "InputManager.h"
#include <SFML/Window/Joystick.hpp>

InputManager& InputManager::getInstance() {
        static InputManager instance;
        return instance;
}

void InputManager::Update() {
        previousKeyboardState = currentKeyboardState;

        // Update the keyboard state
        for (int key = 0; key < sf::Keyboard::KeyCount; ++key) {
                currentKeyboardState[key] = sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(key));
        }
}

bool InputManager::IsKeyDown(const sf::Keyboard::Key key) const {
        return currentKeyboardState.at(key);
}

bool InputManager::IsKeyUp(const sf::Keyboard::Key key) const {
        return !currentKeyboardState.at(key);
}

bool InputManager::IsKeyPressed(const sf::Keyboard::Key key) const {
        return currentKeyboardState.at(key) && !previousKeyboardState.at(key);
}

bool InputManager::IsKeyReleased(const sf::Keyboard::Key key) const {
        return !currentKeyboardState.at(key) && previousKeyboardState.at(key);
}

InputManager::InputManager() {
        // Initialize the keyboard state
        for (int key = sf::Keyboard::A; key <= sf::Keyboard::KeyCount; ++key) {
                currentKeyboardState[key] = false;
                previousKeyboardState[key] = false;
        }
}