#include <iostream>
#include "Game.h"
#include "managers/AssetManager.cpp"

int main(int argc, char* argv[]) {

    // Load it first otherwise it glitches the game for a moment.
    AssetManager const& _ =  AssetManager::getInstance();

    // Run the game
    Game game;
    game.Run();

    // Goodbye world!
    std::cout << "Thanks for playing <3" << std::endl;
    return 0;
}
