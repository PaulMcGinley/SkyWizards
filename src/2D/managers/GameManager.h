//
// Created by Paul McGinley on 23/04/2025.
//

#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <SFML/Graphics/RenderWindow.hpp>
#include <string>
#include <SFML/System/Vector2.hpp>
#include <algorithm> // Needed for std::clamp in setUIScale

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

#endif //GAMEMANAGER_H