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
        std::string exeDir = getExecutableDirectory();
        unsigned int centerScreenX = game_manager.getResolutionWidth() / 2;

        // Small delay before starting the game, this allows the window to load without being a black screen
        if (!gameTime.TimeElapsed(1)) // 1 second
                return;

        // Robe Textures
        if (asset_manager.robes.empty()) {
                asset_manager.robes.push_back(TextureLibrary(exeDir + "/resources/RobeBlue.lib"));
                asset_manager.robes[0].LoadIndices({});
                asset_manager.robes.push_back(TextureLibrary(exeDir + "/resources/RobeGreen.lib"));
                asset_manager.robes[1].LoadIndices({});
                asset_manager.robes.push_back(TextureLibrary(exeDir + "/resources/RobePurple.lib"));
                asset_manager.robes[2].LoadIndices({});

                CurrentValue+=3;
                text.setString("Loading Staff Textures...");
                text.setPosition(centerScreenX - (text.getGlobalBounds().width/2), textYPosition);
                return;
        }

        // Staff Textures
        if (asset_manager.staffs.empty()) {
                asset_manager.staffs.push_back(TextureLibrary(exeDir + "/resources/Staff1.lib"));
                asset_manager.staffs[0].LoadIndices({});
                asset_manager.staffs.push_back(TextureLibrary(exeDir + "/resources/Staff2.lib"));
                asset_manager.staffs[1].LoadIndices({});
                asset_manager.staffs.push_back(TextureLibrary(exeDir + "/resources/Staff3.lib"));
                asset_manager.staffs[2].LoadIndices({});

                CurrentValue+=3;
                text.setString("Loading Heart Textures...");
                text.setPosition(centerScreenX - (text.getGlobalBounds().width/2), textYPosition);
                return;
        }

        // Heart Textures
        if (asset_manager.hearts.entryCount == 0) {
                asset_manager.hearts = TextureLibrary(exeDir + "/resources/Hearts.lib");
                asset_manager.hearts.LoadIndices({});
                CurrentValue++;
                text.setString("Loading Game UI Elements!");
                text.setPosition(centerScreenX - (text.getGlobalBounds().width/2), textYPosition);
                return;
        }

        // Program Usage (PrgUse)
        if (asset_manager.programUsage.entryCount == 0) {
                asset_manager.programUsage = TextureLibrary(exeDir + "/resources/PrgUse.lib");
                asset_manager.programUsage.LoadIndices({});
                CurrentValue++;
                text.setString("Loading Level Objects!");
                text.setPosition(centerScreenX - (text.getGlobalBounds().width/2), textYPosition);
                return;
        }

        // Level Objects (OLibrary) XML files
        if (asset_manager.ObjectLibraries.empty()) {
                loadLevelObjects(exeDir + "/resources/levelobjects/");
                CurrentValue++;
                text.setString("Loading Map Objects!");
                text.setPosition(centerScreenX - (text.getGlobalBounds().width/2), textYPosition);
                return;
        }

        // Maps
        if (asset_manager.Maps.empty()) {
                loadMaps(exeDir + "/resources/maps/");
                CurrentValue++;
                text.setString("Loading Complete...!");
                text.setPosition(centerScreenX - (text.getGlobalBounds().width/2), textYPosition);
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
        window.draw(backgroundQuad, &background); // Draw the background
        window.draw(progressQuad, &progress); // Draw the progress bar
        window.draw(frameQuad, &frame); // Draw the frame
        window.draw(text); // Draw the text
        window.draw(copyRightText); // Draw the copy right text
}

void SplashScreen::InitializeScene() {
        std::string exeDir = getExecutableDirectory();

        if (!font.loadFromFile(exeDir + "/resources/fonts/Simple Santa.otf")) {
                // Handle error
        }

        text.setFont(font);
        text.setString("Loading Robe Textures...");
        text.setPosition((game_manager.getResolutionWidth()/2) - (text.getGlobalBounds().width/2), textYPosition);
        text.setCharacterSize(24);
        text.setFillColor(sf::Color::White);
        text.setStyle(sf::Text::Regular);

        copyrightFont.loadFromFile(exeDir + "/resources/fonts/OpenSans-Bold.ttf");

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
