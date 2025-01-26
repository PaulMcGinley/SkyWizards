#include <SFML/Graphics/RenderWindow.hpp>
//
// Created by Paul McGinley on 26/01/2025.
//
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

        const std::string gameName = "Legend of Sky Wizards - But from the side";

        // Pointer to the window object
        sf::RenderWindow* window = nullptr;



        /* ===========================================================================================================================================================================================
         * ===   RESOLUTION   ========================================================================================================================================================================
         * ===========================================================================================================================================================================================
        */

        // ********** Getters **********

        [[nodiscard]] bool isCustomResolution() const {
                return customResolution;
        }

        // Calculate the resolution ratio and return it as a float
        [[nodiscard]] float resolutionRatio() const {
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
        void setResolution(sf::Vector2<unsigned int> newResolution) {
                resolution = newResolution;
                customResolution = true;
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
                return exclusiveFullscreen;
        }

        // ********** Setters **********

        void toggleFullscreen() {
                fullscreen = !fullscreen;
        }

        void toggleExclusiveFullscreen() {
                exclusiveFullscreen = !exclusiveFullscreen;
        }



        /* ===========================================================================================================================================================================================
         * ===   VSYNC   =============================================================================================================================================================================
         * ===========================================================================================================================================================================================
        */

        // ********** Getters **********

        [[nodiscard]] bool isVsync() const {
                return vsync;
        }

        // ********** Setters **********

        void toggleVsync() {
                vsync = !vsync;
        }



        /* ===========================================================================================================================================================================================
         * ===   Scalers   ===========================================================================================================================================================================
         * ===========================================================================================================================================================================================
        */

        // ********** Getters **********
        [[nodiscard]] float getUIScale() const {
                return scaleUI;
        }

        // ********** Setters **********
        void setUIScale(const float scale) {
                // Limit the scale to a minimum of 10% and a maximum of 800%
                scaleUI = std::clamp(scale, 0.1f, 8.0f);
        }



private:
        // Private constructor to prevent instancing
        GameManager() = default;

        bool customResolution = true;
        sf::Vector2<unsigned int> resolution = {1920, 1080};

        bool fullscreen = false;
        bool exclusiveFullscreen = false;
        bool vsync = true;
        bool showFPS = false;
        bool showDebug = false;
        bool showMemory = false;
        bool showCursor = true;

        float scaleUI = 1.0f;
        float scaleGame = 1.0f;
};
