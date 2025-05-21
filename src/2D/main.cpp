#include <iostream>
#include "Game.h"
#include "managers/AssetManager.h"
#include "managers/InputManager.h"
#include "managers/SceneManager.h"
#include "os/GetExecutionDirectory.h"

int main(int argc, char* argv[]) {
        // Call the getInstance() method to ensure the singleton is created before the game starts
        // It is important to call this before the game starts otherwise the graphics will glitch at the start
        AssetManager const& assetManager =  AssetManager::GetInstance();
        GameManager const& gameManager = GameManager::getInstance();
        InputManager const& inputManager = InputManager::getInstance();
        SceneManager const& sceneManager = SceneManager::GetInstance();

        AssetManager::LoadFonts(getExecutableDirectory() + "/resources/fonts/");

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
                GameManager::getInstance().window = new sf::RenderWindow(sf::VideoMode(gameManager.getResolutionWidth(), gameManager.getResolutionHeight()), gameManager.game_name, sf::Style::Fullscreen);
        }
        // If the game is in exclusive fullscreen mode then set the window to exclusive fullscreen
        else if (GameManager::getInstance().isExclusiveFullscreen()) {
                GameManager::getInstance().window->create(sf::VideoMode(gameManager.getResolutionWidth(), gameManager.getResolutionHeight()), gameManager.game_name, sf::Style::Fullscreen);
        }
        // Else set the window to windowed mode
        else {
                GameManager::getInstance().window = new sf::RenderWindow(sf::VideoMode(gameManager.getResolutionWidth(), gameManager.getResolutionHeight()), gameManager.game_name, sf::Style::Default);
        }

        // Run the game
        Game game;
        game.Run();

        // Goodbye world!
        std::cout << "Thanks for playing <3" << std::endl;

        return 0;
}
