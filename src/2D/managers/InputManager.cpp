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

    // Update the keyboard and joystick state
    void Update() {
        previousState = currentState;

        // Update keyboard state
        const int firstKey = sf::Keyboard::A;
        const int lastKey = sf::Keyboard::Space;
        for (int key = firstKey; key <= lastKey; ++key) {
            currentState[key] = sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(key));
        }

        // Update joystick state
        for (unsigned int joystickId = 0; joystickId < sf::Joystick::Count; ++joystickId) {
            if (sf::Joystick::isConnected(joystickId)) {
                for (unsigned int button = 0; button < sf::Joystick::getButtonCount(joystickId); ++button) {
                    joystickState[joystickId][button] = sf::Joystick::isButtonPressed(joystickId, button);
                }
            }
        }
    }

    // Check if a key is currently down
    [[nodiscard]] bool IsKeyDown(const sf::Keyboard::Key key) const {
        return currentState.at(key);
    }

    // Check if a key is currently up
    [[nodiscard]] bool IsKeyUp(const sf::Keyboard::Key key) const {
        return !currentState.at(key);
    }

    // Check if a key was pressed (down now, but not down in the previous state)
    [[nodiscard]] bool IsKeyPressed(const sf::Keyboard::Key key) const {
        return currentState.at(key) && !previousState.at(key);
    }

    [[nodiscard]] bool IsKeyReleased(const sf::Keyboard::Key key) const {
        return !currentState.at(key) && previousState.at(key);
    }

    // Check if a joystick button is currently down
    [[nodiscard]] bool IsJoystickButtonDown(unsigned int joystickId, unsigned int button) const {
        return joystickState.at(joystickId).at(button);
    }

    // Check if a joystick button is currently up
    [[nodiscard]] bool IsJoystickButtonUp(unsigned int joystickId, unsigned int button) const {
        return !joystickState.at(joystickId).at(button);
    }

    // Check if a joystick button was pressed (down now, but not down in the previous state)
    [[nodiscard]] bool IsJoystickButtonPressed(unsigned int joystickId, unsigned int button) const {
        return joystickState.at(joystickId).at(button) && !previousJoystickState.at(joystickId).at(button);
    }

    [[nodiscard]] bool IsJoystickButtonReleased(unsigned int joystickId, unsigned int button) const {
        return !joystickState.at(joystickId).at(button) && previousJoystickState.at(joystickId).at(button);
    }

private:
    // Private constructor to prevent instancing
    InputManager() {
        for (int key = sf::Keyboard::A; key <= sf::Keyboard::KeyCount; ++key) {
            currentState[key] = false;
            previousState[key] = false;
        }
        for (unsigned int joystickId = 0; joystickId < sf::Joystick::Count; ++joystickId) {
            for (unsigned int button = 0; button < sf::Joystick::ButtonCount; ++button) {
                joystickState[joystickId][button] = false;
                previousJoystickState[joystickId][button] = false;
            }
        }
    }

    std::unordered_map<int, bool> currentState;
    std::unordered_map<int, bool> previousState;
    std::unordered_map<unsigned int, std::unordered_map<unsigned int, bool>> joystickState;
    std::unordered_map<unsigned int, std::unordered_map<unsigned int, bool>> previousJoystickState;
};