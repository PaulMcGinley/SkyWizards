//
// Created by Paul McGinley on 23/04/2025.
//

#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm> // Needed for std::clamp in setUIScale
#include <string>

#include "models/Leaderboard.h"

class GameManager {
public:
        // Method to get the instance of the singleton
        static GameManager& GetInstance() {
                static GameManager instance; // Guaranteed to be destroyed.
                // Instantiated on first use.
                return instance;
        }

        // Delete copy constructor and assignment operator to prevent copies
        GameManager(const GameManager&) = delete;
        void operator=(const GameManager&) = delete;

        // Window title
        const std::string GAME_NAME = "Legend of Sky Wizards - But from the side";

        // Pointer to the window object
        // mutable to allow modification of the window in const methods
        mutable sf::RenderWindow* window = nullptr;

        /* ===========================================================================================================================================================================================
         * ===   RESOLUTION   ========================================================================================================================================================================
         * ===========================================================================================================================================================================================
        */

        // ********** Getters **********
        [[nodiscard]] bool isCustomResolution() const;
        [[nodiscard]] float getResolutionRatio() const;
        [[nodiscard]] sf::Vector2<unsigned int> getResolution() const;
        [[nodiscard]] unsigned int getResolutionWidth() const;
        [[nodiscard]] unsigned int getResolutionHeight() const;

        // ********** Setters **********
        void setResolution(const sf::Vector2<unsigned int> new_resolution);

        /* ===========================================================================================================================================================================================
         * ===   FULL SCREEN   =======================================================================================================================================================================
         * ===========================================================================================================================================================================================
        */

        // ********** Getters **********
        [[nodiscard]] bool isFullscreen() const;
        [[nodiscard]] bool isExclusiveFullscreen() const;

        // ********** Setters **********
        void toggleFullscreen();
        void toggleExclusiveFullscreen();

        /* ===========================================================================================================================================================================================
         * ===   VSYNC   =============================================================================================================================================================================
         * ===========================================================================================================================================================================================
        */

        // ********** Getters **********
        [[nodiscard]] bool isVSyncEnabled() const;

        // ********** Setters **********
        void toggleVsync();

        /* ===========================================================================================================================================================================================
         * ===   SCALE   =============================================================================================================================================================================
         * ===========================================================================================================================================================================================
        */

        // ********** Getters **********
        [[nodiscard]] float getUIScale() const;

        // ********** Setters **********
        void setUIScale(float scale);



        void SetLastPlayedMap(const std::string& mapName) {
                lastPlayedMap = mapName;
        }

        [[nodiscard]] std::string GetLastPlayedMap() const {
                return lastPlayedMap;
        }


        bool Debug() const { return show_debug; }
        void ToggleDebug() { show_debug = !show_debug; }

        bool ShowCollisions() const { return showCollisions; }
        void ToggleShowCollisions() { showCollisions = !showCollisions; }

        Leaderboard leaderboard;


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
        bool showCollisions = false;
        bool show_memory = false;
        bool show_cursor = true;

        float scale_ui = 1.0f;
        float scale_game = 1.0f;

        std::string lastPlayedMap = "";
};

#endif //GAMEMANAGER_H