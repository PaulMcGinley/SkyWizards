//
// Created by Paul McGinley on 09/12/2024.
//

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Joystick.hpp>
#include <unordered_map>

class InputManager {
public:
        // Method to get the instance of the singleton
        static InputManager& getInstance() {
                static InputManager instance;
                return instance;
        }

        // Delete copy constructor and assignment operator to prevent copies
        InputManager(const InputManager&) = delete;
        void operator=(const InputManager&) = delete;

        // Update the input device states
        void Update() {
                previousKeyboardState = currentKeyboardState;

                // Update keyboard state
                for (int key = 0; key < sf::Keyboard::KeyCount; ++key) {
                        currentKeyboardState[key] = sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(key));
                }
        }

        // Check if a key is currently down
        [[nodiscard]] bool IsKeyDown(const sf::Keyboard::Key key) const {
                return currentKeyboardState.at(key);
        }

        // Check if a key is currently up
        [[nodiscard]] bool IsKeyUp(const sf::Keyboard::Key key) const {
                return !currentKeyboardState.at(key);
        }

        // Check if a key was pressed (down now, but not down in the previous state)
        [[nodiscard]] bool IsKeyPressed(const sf::Keyboard::Key key) const {
                return currentKeyboardState.at(key) && !previousKeyboardState.at(key);
        }

        [[nodiscard]] bool IsKeyReleased(const sf::Keyboard::Key key) const {
                return !currentKeyboardState.at(key) && previousKeyboardState.at(key);
        }

private:
        // Private constructor to prevent instancing
        InputManager() {
                // Initialize the keyboard state
                for (int key = sf::Keyboard::A; key <= sf::Keyboard::KeyCount; ++key) {
                        currentKeyboardState[key] = false;
                        previousKeyboardState[key] = false;
                }
        }

        std::unordered_map<int, bool> currentKeyboardState;
        std::unordered_map<int, bool> previousKeyboardState;
};
