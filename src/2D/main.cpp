#include <iostream>
#include "Game.h"
#include "managers/AssetManager.h"
//#include "managers/GameManager.cpp"   // This is included in Game.h
#include "managers/InputManager.cpp"
#include "managers/SceneManager.cpp"

int main(int argc, char* argv[]) {

        // Call the getInstance() method to ensure the singleton is created before the game starts
        AssetManager const& asset_manager =  AssetManager::getInstance();       // It is important to call this before the game starts otherwise the graphics will glitch at the start
        GameManager const& game_manager = GameManager::getInstance();
        InputManager const& input_manager = InputManager::getInstance();
        SceneManager const& scene_manager = SceneManager::getInstance();

        // TODO: Read settings from file

        // If the resolution has been set by the user then use that resolution
        if (GameManager::getInstance().isCustomResolution()) {
                GameManager::getInstance().setResolution({1920, 1080}); // TODO: Change to read from file
        }
        // Else use the monitor resolution as the game resolution
        else {
                // Get monitor resolution
                sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
                // Set the games resolution to the desktop resolution
                GameManager::getInstance().setResolution({desktop.width, desktop.height});
        }

        // If the game is in fullscreen mode then set the window to fullscreen
        if (GameManager::getInstance().isFullscreen()) {
                GameManager::getInstance().window = new sf::RenderWindow(sf::VideoMode(game_manager.getResolutionWidth(), game_manager.getResolutionHeight()), game_manager.game_name, sf::Style::Fullscreen);
        }
        // If the game is in exclusive fullscreen mode then set the window to exclusive fullscreen
        else if (GameManager::getInstance().isExclusiveFullscreen()) {
                GameManager::getInstance().window->create(sf::VideoMode(game_manager.getResolutionWidth(), game_manager.getResolutionHeight()), game_manager.game_name, sf::Style::Fullscreen);
        }
        // Else set the window to windowed mode
        else {
                GameManager::getInstance().window = new sf::RenderWindow(sf::VideoMode(game_manager.getResolutionWidth(), game_manager.getResolutionHeight()), game_manager.game_name, sf::Style::Default);
        }

        // Run the game
        Game game;
        game.Run();

        // Goodbye world!
        std::cout << "Thanks for playing <3" << std::endl;

        return 0;
}
