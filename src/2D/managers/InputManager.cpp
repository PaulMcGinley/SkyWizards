//
// Created by Paul McGinley on 09/12/2024.
//

#include "InputManager.h"
#include <SFML/Window/Joystick.hpp>

InputManager& InputManager::GetInstance() {
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

bool InputManager::IsKeyDown(const sf::Keyboard::Key key) const { return currentKeyboardState.at(key); }
bool InputManager::IsKeyUp(const sf::Keyboard::Key key) const { return !currentKeyboardState.at(key); }
bool InputManager::IsKeyPressed(const sf::Keyboard::Key key) const { return currentKeyboardState.at(key) && !previousKeyboardState.at(key); }
bool InputManager::IsKeyReleased(const sf::Keyboard::Key key) const { return !currentKeyboardState.at(key) && previousKeyboardState.at(key); }
bool InputManager::IsJumpPressed() const {
        return IsKeyDown(sf::Keyboard::Space)           // PC
        || IsKeyDown(sf::Keyboard::Num4)                // Left controls
        || IsKeyDown(sf::Keyboard::Numpad4)             // Left controls
        || IsKeyDown(sf::Keyboard::J);                  // Right controls
}
bool InputManager::IsConfirmPressed() const {
        return IsKeyPressed(sf::Keyboard::Space)        // PC
        || IsKeyPressed(sf::Keyboard::Num4)             // Left controls
        || IsKeyPressed(sf::Keyboard::Numpad4)          // Left controls
        || IsKeyPressed(sf::Keyboard::J);               // Right controls
}
bool InputManager::IsCancelPressed() const {
        return IsKeyPressed(sf::Keyboard::Backspace)    // PC
        || IsKeyPressed(sf::Keyboard::Num7)             // Left controls
        || IsKeyPressed(sf::Keyboard::Numpad7)          // Left controls
        || IsKeyPressed(sf::Keyboard::I);               // Right controls
}
bool InputManager::IsFirePressed() const {
        return IsKeyDown(sf::Keyboard::LShift)          // PC
        || IsKeyDown(sf::Keyboard::Num5)                // Left controls
        || IsKeyDown(sf::Keyboard::Numpad5)             // Left controls
        || IsKeyDown(sf::Keyboard::Num6)                // Left controls
        || IsKeyDown(sf::Keyboard::Numpad6)             // Left controls
        || IsKeyDown(sf::Keyboard::Num8)                // Left controls
        || IsKeyDown(sf::Keyboard::Numpad8)             // Left controls
        || IsKeyDown(sf::Keyboard::Num9)                // Left controls
        || IsKeyDown(sf::Keyboard::Numpad9)             // Left controls
        || IsKeyDown(sf::Keyboard::K)                   // Right controls
        || IsKeyDown(sf::Keyboard::L)                   // Right controls
        || IsKeyDown(sf::Keyboard::O)                   // Right controls
        || IsKeyDown(sf::Keyboard::P);                  // Right controls
}
bool InputManager::NavigateUpPressed() const {
        return IsKeyPressed(sf::Keyboard::Up)           // Left controls
        || IsKeyPressed(sf::Keyboard::W);               // Right controls
}
bool InputManager::NavigateDownPressed() const {
        return IsKeyPressed(sf::Keyboard::Down)         // Left controls
        || IsKeyPressed(sf::Keyboard::S);               // Right controls
}
bool InputManager::MoveLeftPressed() const {
        return IsKeyDown(sf::Keyboard::Left)            // Left controls
        || IsKeyDown(sf::Keyboard::A);                  // Right controls
}
bool InputManager::MoveRightPressed() const {
        return IsKeyDown(sf::Keyboard::Right)           // Left controls
        || IsKeyDown(sf::Keyboard::D);                  // Right controls
}
bool InputManager::MainMenuPressed() const {
        return IsKeyPressed(sf::Keyboard::Num1)         // Left controls
        || IsKeyPressed(sf::Keyboard::Numpad1)          // Left controls
        || IsKeyPressed(sf::Keyboard::N);               // Right controls
}
bool InputManager::ShowCollisionBoxsPressed() const {
        return IsKeyDown(sf::Keyboard::Up)              // Hold
        && IsKeyPressed(sf::Keyboard::W);               // Press
}
bool InputManager::ShowDebugPressed() const {
        return IsKeyDown(sf::Keyboard::Down) // Hold
               && IsKeyPressed(sf::Keyboard::S); // Press
}

InputManager::InputManager() {
        // Initialize the keyboard state
        for (int key = sf::Keyboard::A; key <= sf::Keyboard::KeyCount; ++key) {
                currentKeyboardState[key] = false;
                previousKeyboardState[key] = false;
        }
}
