//
// Created by Paul McGinley on 08/10/2024.
//

#include "SplashScreen.h"

#include <3rdParty/pugixml/pugixml.hpp>
#include <filesystem>

#include "managers/SceneManager.h"
#include "models/LevelObject/OLibrary.h"
#include "models/MapObject/WMap.h"
#include "os/GetExecutionDirectory.h"

SplashScreen::SplashScreen() {
        InitializeLibraries(); // Load the libraries
        // Constructor
}

void SplashScreen::Update(GameTime gameTime) {
        const std::string exeDir = getExecutableDirectory();

        // Small delay before starting the game, this allows the window to load without being a black screen
        if (!gameTime.TimeElapsed(1)) // 1 second
                return;

        if (!asset_manager.TextureLibraries["RobeBlue"]->fullyLoaded) {
                asset_manager.TextureLibraries["RobeBlue"].get()->LoadIndices({});
                CurrentValue++;
                return;
        }

        if (!asset_manager.TextureLibraries["RobeGreen"]->fullyLoaded) {
                asset_manager.TextureLibraries["RobeGreen"].get()->LoadIndices({});
                CurrentValue++;
                return;
        }

        if (!asset_manager.TextureLibraries["RobePurple"]->fullyLoaded) {
                asset_manager.TextureLibraries["RobePurple"].get()->LoadIndices({});
                CurrentValue++;
                return;
        }

        if (!asset_manager.TextureLibraries["Staff1"]->fullyLoaded) {
                asset_manager.TextureLibraries["Staff1"].get()->LoadIndices({});
                CurrentValue++;
                return;
        }

        if (!asset_manager.TextureLibraries["Staff2"]->fullyLoaded) {
                asset_manager.TextureLibraries["Staff2"].get()->LoadIndices({});
                CurrentValue++;
                return;
        }

        if (!asset_manager.TextureLibraries["Staff3"]->fullyLoaded) {
                asset_manager.TextureLibraries["Staff3"].get()->LoadIndices({});
                CurrentValue++;
                return;
        }

        if (!asset_manager.TextureLibraries["hearts"]->fullyLoaded) {
                asset_manager.TextureLibraries["hearts"].get()->LoadIndices({});
                CurrentValue++;
                return;
        }

        // Program Usage (PrgUse)
        if (!asset_manager.TextureLibraries["PrgUse"]->fullyLoaded) {
                asset_manager.TextureLibraries["PrgUse"].get()->LoadIndices({});
                CurrentValue++;
                return;
        }

        if (!asset_manager.TextureLibraries["magic"]->fullyLoaded) {
                asset_manager.TextureLibraries["magic"].get()->LoadIndices({});
                CurrentValue++;
                return;
        }

        // Level Objects (OLibrary) XML files
        if (asset_manager.ObjectLibraries.empty()) {
                loadLevelObjects(exeDir + "/resources/levelobjects/");
                CurrentValue++;
                return;
        }

        // Maps
        if (asset_manager.Maps.empty()) {
                loadMaps(exeDir + "/resources/maps/");
                CurrentValue++;
                return;
        }

        // We can only get to this point if everything is loaded into memory
        // Track when loaded finished so we can add a small delay before starting the game
        if (loadCompletionTime < 0) {
                loadCompletionTime = gameTime.NowAddMilliseconds(250);
        }
}

void SplashScreen::LateUpdate(GameTime gameTime) {
        if (loadCompletionTime < 0) {
                return;
        }

        if (gameTime.TimeElapsed(loadCompletionTime)) {
                // Load the next scene
                std::cout << "Persistent assets loaded." << std::endl;
                SceneManager::GetInstance().ChangeScene(SceneType::SCENE_MAIN_MENU);
        }
}

void SplashScreen::Draw(sf::RenderWindow& window, GameTime gameTime) {
        // Draw the background
        window.draw(backgroundQuad, &background);

        // Calculate progress percentage
        float progressPercentage = static_cast<float>(CurrentValue) / static_cast<float>(TargetValue);

        // Frame sprite
        sf::Sprite frameSprite(frame);
        float frameX = (game_manager.getResolutionWidth() - frameSprite.getGlobalBounds().width) / 2;
        float frameY = game_manager.getResolutionHeight() * 0.75f;
        frameSprite.setPosition(frameX, frameY);


        // Progress bar
        sf::RectangleShape progressBar;
        float progressBarWidth = frameSprite.getGlobalBounds().width * 0.9f;
        float progressBarHeight = frameSprite.getGlobalBounds().height * 0.4f;

        float progressX = frameX + (frameSprite.getGlobalBounds().width - progressBarWidth) / 2;
        float progressY = frameY + (frameSprite.getGlobalBounds().height - progressBarHeight) / 2;

        progressBar.setSize(sf::Vector2f(progressBarWidth * progressPercentage, progressBarHeight));
        progressBar.setPosition(progressX, progressY);
        progressBar.setFillColor(sf::Color(50, 50, 180));

        window.draw(progressBar);
        window.draw(frameSprite);
        window.draw(copyRightText);
}

void SplashScreen::InitializeScene() {
        std::string exeDir = getExecutableDirectory();

        copyrightFont = *asset_manager.Fonts["OpenSans-Bold"].get();

        copyRightText.setFont(copyrightFont);
        copyRightText.setString("Created by Paul McGinley on 08/10/2024.");
        copyRightText.setPosition(15, 1045);
        copyRightText.setCharacterSize(18);
        copyRightText.setFillColor(sf::Color::White);


        background.loadFromFile(exeDir + "/resources/loader/background.png");
        frame.loadFromFile(exeDir + "/resources/loader/frame.png");
        progress.loadFromFile(exeDir + "/resources/loader/progressbar.png");

        backgroundQuad[0].position = {0, 0};
        backgroundQuad[1].position = {1920, 0};
        backgroundQuad[2].position = {1920, 1080};
        backgroundQuad[3].position = {0, 1080};
        backgroundQuad[0].texCoords = {0, 0};
        backgroundQuad[1].texCoords = {static_cast<float>(background.getSize().x), 0};
        backgroundQuad[2].texCoords = {static_cast<float>(background.getSize().x), static_cast<float>(background.getSize().y)};
        backgroundQuad[3].texCoords = {0, static_cast<float>(background.getSize().y)};

        //Frame needs to be centre and 300px from the bottom
        float frameWidth = static_cast<float>(frame.getSize().x);
        float frameHeight = static_cast<float>(frame.getSize().y);

        sf::Vector2f frameStartPic = {518,850   };
        frameQuad[0].position = {frameStartPic.x, frameStartPic.y};
        frameQuad[1].position = {frameStartPic.x + frameWidth, frameStartPic.y};
        frameQuad[2].position = {frameStartPic.x + frameWidth, frameStartPic.y + frameHeight};
        frameQuad[3].position = {frameStartPic.x, frameStartPic.y + frameHeight};

        frameQuad[0].texCoords = {0, 0};
        frameQuad[1].texCoords = {frameWidth, 0};
        frameQuad[2].texCoords = {frameWidth, frameHeight};
        frameQuad[3].texCoords = {0, frameHeight};


        // progress bar needs to be 10px from the bottom and 10px from the left
        float progressWidth = static_cast<float>(progress.getSize().x);
        float progressHeight = static_cast<float>(progress.getSize().y);

        sf::Vector2f progressStartPic = {562,893};
        progressQuad[0].position = {progressStartPic.x, progressStartPic.y};
        progressQuad[1].position = {progressStartPic.x + progressWidth, progressStartPic.y};
        progressQuad[2].position = {progressStartPic.x + progressWidth, progressStartPic.y + progressHeight};
        progressQuad[3].position = {progressStartPic.x, progressStartPic.y + progressHeight};

        progressQuad[0].texCoords = {0, 0};
        progressQuad[1].texCoords = {static_cast<float>(progress.getSize().x), 0};
        progressQuad[2].texCoords = {static_cast<float>(progress.getSize().x), static_cast<float>(progress.getSize().y)};
        progressQuad[3].texCoords = {0, static_cast<float>(progress.getSize().y)};

        IScene::InitializeScene(); // Call the parent class to set the scene as initialized
}

void SplashScreen::DestroyScene() {
        // When the scene is finished
}

void SplashScreen::OnScene_Active() {
        // When the scene starts
}

void SplashScreen::OnScene_Deactivate() {
        // When the scene ends
}

// Load all the .lib files into the map
// This will initialize the arrays in the AssetManager and populate the FAT
// No image data is loaded at this point
void SplashScreen::InitializeLibraries() {
        std::string resourcesDir = getExecutableDirectory() + "/resources/";

        // Get all .lib files in the resources directory
        for (const auto &entry: std::filesystem::directory_iterator(resourcesDir)) {
                if (entry.is_regular_file() && entry.path().extension() == ".lib") {
                        std::string filePath = entry.path().string();
                        std::string fileNameWithoutExtension = entry.path().stem().string();

                        // Load the library
                        asset_manager.LoadTextureLibrary(filePath);
                }
        }
}

void SplashScreen::loadLevelObjects(const std::string &directoryPath) {
        // Load all the Olibrary files
        for (const auto &entry: std::filesystem::directory_iterator(directoryPath)) {
                if (entry.is_regular_file() && entry.path().extension() == ".olx") {
                        std::string filePath = entry.path().string();
                        std::string fileNameWithoutExtension = entry.path().stem().string();

                        pugi::xml_document doc;
                        pugi::xml_parse_result result = doc.load_file(filePath.c_str());

                        if (result) {
                                OLibrary objectLibrary;
                                if (objectLibrary.deserialize(doc)) {
                                        asset_manager.ObjectLibraries[fileNameWithoutExtension] = std::make_unique<OLibrary>(std::move(objectLibrary));
                                } else {
                                        std::cerr << "Failed to deserialize Olibrary: " << filePath << std::endl;
                                }
                        } else {
                                std::cerr << "Failed to load OLibrary: " << filePath
                                          << " Error: " << result.description() << std::endl;
                        }
                }
        }
}

void SplashScreen::loadMaps(const std::string &directoryPath) {
        // Load all the map files
        for (const auto &entry: std::filesystem::directory_iterator(directoryPath)) {
                if (entry.is_regular_file() && entry.path().extension() == ".wmap") {
                        std::string filePath = entry.path().string();
                        std::string fileNameWithoutExtension = entry.path().stem().string();

                        pugi::xml_document doc;
                        pugi::xml_parse_result result = doc.load_file(filePath.c_str());

                        if (result) {
                                auto map = std::make_unique<WMap>();
                                if (map->deserialize(doc)) {
                                        asset_manager.Maps[fileNameWithoutExtension] = std::move(map);
                                } else {
                                        std::cerr << "Failed to deserialize Map: " << filePath << std::endl;
                                }
                        } else {
                                std::cerr << "Failed to load Map: " << filePath << " Error: " << result.description()
                                          << std::endl;
                        }
                }
        }
}
