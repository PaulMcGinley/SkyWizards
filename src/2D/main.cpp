#include <__filesystem/operations.h>
#include <iostream>

#include "Game.h"
#include "IO/GameSettingsFile.h"
#include "managers/AssetManager.h"
#include "managers/InputManager.h"
#include "managers/SceneManager.h"
#include "os/GetExecutionDirectory.h"

int main(int argc, char* argv[]) {
        // Call the getInstance() method to ensure the singleton is created before the game starts
        // It is important to call this before the game starts otherwise the graphics will glitch at the start
        AssetManager & assetManager =  AssetManager::GetInstance();
        GameManager & gameManager = GameManager::getInstance();
        InputManager & inputManager = InputManager::getInstance();
        SceneManager & sceneManager = SceneManager::GetInstance();

        std::string musicDir = getExecutableDirectory() + "/resources/loader/";
        assetManager.LoadMusic(musicDir);
        assetManager.PlayMusic("song", true); // Play the loader music

        std::string leaderBoardDir = getExecutableDirectory() + "/leaderboard/";
        // Create the leaderboard directory if it doesn't exist
        if (!std::filesystem::exists(leaderBoardDir)) {
                std::filesystem::create_directory(leaderBoardDir);
        }

        // We need to load the fonts before we initialize any scene as some scenes use the fonts to draw text.
        AssetManager::LoadFonts(getExecutableDirectory() + "/resources/fonts/");

        // Read GameSettings from file
        GameSettingsFile settingsFile;
        settingsFile.LoadSettings();

        // gameManager.setResolution({settingsFile.GetResolutionWidth(), settingsFile.GetResolutionHeight()});
        // gameManager.setFrameRateLimit(settingsFile.GetFrameRateLimit());
        // gameManager.setFullscreen(settingsFile.IsFullscreenMode());
        // gameManager.setVSync(settingsFile.IsVsyncEnabled());

        // Setup the window
        gameManager.window = new sf::RenderWindow(sf::VideoMode(gameManager.getResolutionWidth(), gameManager.getResolutionHeight()), gameManager.GAME_NAME, sf::Style::Default);
        gameManager.window->setVerticalSyncEnabled(true);
        gameManager.window->setFramerateLimit(120); // Keep inder 600 to stop physics issues
        gameManager.window->setMouseCursorVisible(false);
        gameManager.window->setActive(true);

        // Run the game
        Game game;
        game.Run();

        // Goodbye world!
        std::cout << "Thanks for playing <3" << std::endl;

        return 0;
}
