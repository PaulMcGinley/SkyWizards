//
// Created by Paul McGinley on 26/01/2025.
//
#pragma once // Inclusion guard to prevent multiple includes

#include <SFML/Graphics/RenderWindow.hpp>
#include <string>
#include <SFML/System/Vector2.hpp>
#include <algorithm>

class GameManager {
public:
        // Method to get the instance of the singleton
        static GameManager& getInstance() {
                static GameManager instance; // Guaranteed to be destroyed.
                // Instantiated on first use.
                return instance;
        }

        // Delete copy constructor and assignment operator to prevent copies
        GameManager(const GameManager&) = delete;
        void operator=(const GameManager&) = delete;

        // Window title
        const std::string game_name = "Legend of Sky Wizards - But from the side";

        // Pointer to the window object
        sf::RenderWindow* window = nullptr;



        /* ===========================================================================================================================================================================================
         * ===   RESOLUTION   ========================================================================================================================================================================
         * ===========================================================================================================================================================================================
        */

        // ********** Getters **********

        [[nodiscard]] bool isCustomResolution() const {
                return is_custom_resolution;
        }

        // Calculate the resolution ratio and return it as a float
        [[nodiscard]] float getResolutionRatio() const {
                return static_cast<float>(resolution.x) / static_cast<float>(resolution.y);
        }

        // Get the resolution as a vector
        [[nodiscard]] sf::Vector2<unsigned int> getResolution() const {
                return resolution;
        }

        // Get the resolution width
        [[nodiscard]] unsigned int getResolutionWidth() const {
                return resolution.x;
        }

        // Get the resolution height
        [[nodiscard]] unsigned int getResolutionHeight() const {
                return resolution.y;
        }

        // ********** Setters **********

        // Set the resolution
        void setResolution(const sf::Vector2<unsigned int> new_resolution) {
                resolution = new_resolution;
                is_custom_resolution = true;
        }



        /* ===========================================================================================================================================================================================
         * ===   FULL SCREEN   =======================================================================================================================================================================
         * ===========================================================================================================================================================================================
        */

        // ********** Getters **********

        [[nodiscard]] bool isFullscreen() const {
                return fullscreen;
        }

        [[nodiscard]] bool isExclusiveFullscreen() const {
                return exclusive_fullscreen;
        }

        // ********** Setters **********

        void toggleFullscreen() {
                fullscreen = !fullscreen;
        }

        void toggleExclusiveFullscreen() {
                exclusive_fullscreen = !exclusive_fullscreen;
        }



        /* ===========================================================================================================================================================================================
         * ===   VSYNC   =============================================================================================================================================================================
         * ===========================================================================================================================================================================================
        */

        // ********** Getters **********

        [[nodiscard]] bool isVSyncEnabled() const {
                return vsync;
        }

        // ********** Setters **********

        void toggleVsync() {
                vsync = !vsync;
        }



        /* ===========================================================================================================================================================================================
         * ===   SCALE   =============================================================================================================================================================================
         * ===========================================================================================================================================================================================
        */

        // ********** Getters **********

        [[nodiscard]] float getUIScale() const {
                return scale_ui;
        }

        // ********** Setters **********

        void setUIScale(const float scale) {
                // Limit the scale to a minimum of 10% and a maximum of 800%
                scale_ui = std::clamp(scale, 0.1f, 8.0f);
        }



private:
        // Private constructor to prevent instancing
        GameManager() = default;

        bool is_custom_resolution = true;
        sf::Vector2<unsigned int> resolution = {1920, 1080};

        bool fullscreen = false;
        bool exclusive_fullscreen = false;
        bool vsync = true;
        bool show_fps = false;
        bool show_debug = false;
        bool show_memory = false;
        bool show_cursor = true;

        float scale_ui = 1.0f;
        float scale_game = 1.0f;
};
