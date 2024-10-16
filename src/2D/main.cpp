#include <iostream>
#include <string>

#include "Game.h"
#include "managers/AssetManager.cpp"

int main(int argc, char* argv[]) {
    // // Example of using command-line arguments
    // std::cout << "Number of arguments: " << argc << std::endl;
    // for (int i = 0; i < argc; ++i) {
    //     std::cout << "Argument " << i << ": " << argv[i] << std::endl;
    // }
    //
    // // Check for specific arguments
    // bool debugMode = false;
    // bool arcadeMode = false;
    // std::string configFilePath = "empty";
    //
    // for (int i = 1; i < argc; ++i) {
    //     // Debug / Release configuration
    //     if (std::string(argv[i]) == "--debug") {
    //         debugMode = true;
    //     } else if (std::string(argv[i]) == "--config" && i + 1 < argc) {
    //         configFilePath = argv[i + 1];
    //         ++i; // Skip the next argument as it is the value for --config
    //     }
    //
    //     if (std::string(argv[i]) == "arcade") {
    //         arcadeMode = true;
    //     }
    // }
    //
    // if (debugMode) {
    //     std::cout << "Debug mode is ON" << std::endl;
    // }
    //
    // if (arcadeMode) {
    //     // TODO : Set user inputs
    // }
    //
    // if (!configFilePath.empty()) {
    //     std::cout << "Config file path: " << configFilePath << std::endl;
    // }
    // Load it here to stop frozen window
    // TODO : Load libraries as required or with a background worker
    AssetManager& am =  AssetManager::getInstance();

    Game game;
    game.Run();

    std::cout << "Hello, World!" << std::endl;
    return 0;
}