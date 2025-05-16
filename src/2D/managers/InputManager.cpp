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
        void update() {
                previous_state = current_state;

                // Update keyboard state
                // constexpr int first_key = sf::Keyboard::A;
                // constexpr int last_key = sf::Keyboard::Space;
                for (int key = 0; key < sf::Keyboard::KeyCount; ++key) {
                        current_state[key] = sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(key));
                }

                // Update joystick state
                for (unsigned int joystick_id = 0; joystick_id < sf::Joystick::Count; ++joystick_id) {
                        if (sf::Joystick::isConnected(joystick_id)) {
                                for (unsigned int button = 0; button < sf::Joystick::getButtonCount(joystick_id); ++button) {
                                        joystick_state[joystick_id][button] = sf::Joystick::isButtonPressed(joystick_id, button);
                                }
                        }
                }
        }

        // Check if a key is currently down
        [[nodiscard]] bool isKeyDown(const sf::Keyboard::Key key) const {
                return current_state.at(key);
        }

        // Check if a key is currently up
        [[nodiscard]] bool isKeyUp(const sf::Keyboard::Key key) const {
                return !current_state.at(key);
        }

        // Check if a key was pressed (down now, but not down in the previous state)
        [[nodiscard]] bool isKeyPressed(const sf::Keyboard::Key key) const {
                return current_state.at(key) && !previous_state.at(key);
        }

        [[nodiscard]] bool isKeyReleased(const sf::Keyboard::Key key) const {
                return !current_state.at(key) && previous_state.at(key);
        }

        // Check if a joystick button is currently down
        [[nodiscard]] bool isJoystickButtonDown(unsigned int joystick_id, unsigned int button) const {
                return joystick_state.at(joystick_id).at(button);
        }

        // Check if a joystick button is currently up
        [[nodiscard]] bool isJoystickButtonUp(unsigned int joystick_id, unsigned int button) const {
                return !joystick_state.at(joystick_id).at(button);
        }

        // Check if a joystick button was pressed (down now, but not down in the previous state)
        [[nodiscard]] bool isJoystickButtonPressed(unsigned int joystick_id, unsigned int button) const {
                return joystick_state.at(joystick_id).at(button) && !previous_joystick_state.at(joystick_id).at(button);
        }

        [[nodiscard]] bool isJoystickButtonReleased(unsigned int joystick_id, unsigned int button) const {
                return !joystick_state.at(joystick_id).at(button) && previous_joystick_state.at(joystick_id).at(button);
        }

private:
        // Private constructor to prevent instancing
        InputManager() {
                // Initialize the keyboard state
                for (int key = sf::Keyboard::A; key <= sf::Keyboard::KeyCount; ++key) {
                        current_state[key] = false;
                        previous_state[key] = false;
                }
                for (unsigned int joystickId = 0; joystickId < sf::Joystick::Count; ++joystickId) {
                        for (unsigned int button = 0; button < sf::Joystick::ButtonCount; ++button) {
                                joystick_state[joystickId][button] = false;
                                previous_joystick_state[joystickId][button] = false;
                        }
                }
        }

        std::unordered_map<int, bool> current_state;
        std::unordered_map<int, bool> previous_state;
        std::unordered_map<unsigned int, std::unordered_map<unsigned int, bool>> joystick_state;
        std::unordered_map<unsigned int, std::unordered_map<unsigned int, bool>> previous_joystick_state;
};
