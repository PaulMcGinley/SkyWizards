//
// Created by Paul McGinley on 24/04/2025.
//

#include "GameScene.h"

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <iostream>
#include <unordered_map>
#include <unordered_set>

#include "Overlays/DebugOverlay.h"
#include "managers/SceneManager.h"
#include "models/LevelObject/OLibrary.h"
#include "models/MapObject/WMap.h"

GameScene::GameScene()
        : IScene()
        , UpdateLoop(&GameScene::Update_Loading)
        , map(nullptr)
{ /* Nothing in the constructor */}

void GameScene::LoadMap(std::string name)  {
        mapName = std::move(name);
        LoadAssets();
        SpawnPlayer();
}

void GameScene::Update(const GameTime gameTime) {
        (this->*UpdateLoop)(gameTime);

        // DEBUG:
        // if (const auto debugOverlay = scene_manager.GetScene(SceneType::SCENE_DEBUG_OVERLAY)) {
        //         if (const auto debugOverlayPtr = std::dynamic_pointer_cast<DebugOverlay>(debugOverlay)) {
        //                 debugOverlayPtr->AddInfoTopLeft("Player X", std::to_string(player.position.x));
        //                 debugOverlayPtr->AddInfoTopLeft("Player Y", std::to_string(player.position.y));
        //                 debugOverlayPtr->AddInfoTopLeft("Player Velocity X", std::to_string(player.velocity.x));
        //                 debugOverlayPtr->AddInfoTopLeft("Player Velocity Y", std::to_string(player.velocity.y));
        //         }
        // }
        // END DEBUG ^
}

void GameScene::LateUpdate(const GameTime gameTime) {
        player.LateUpdate(gameTime);
        viewport.setCenter(player.position + sf::Vector2f(250,0)); // Center the viewport on the player

        // Call LateUpdate for each Mobj
        for (auto const & mob: monsters) {
                mob->LateUpdate(gameTime);
        }
}

void GameScene::Draw(sf::RenderWindow &window, GameTime gameTime) {
        if (!IsInitialized()) {
                // Draw loading screen
                return;
        }

        CalculateParallaxBackground();
        window.setView(viewport);
        window.draw(skyBoxSprite);


        // Nice to have feature
        // Could tile accross the sky, alter the opacity with sin (0.1 - 0.3)
        // window.setView(window.getDefaultView());
        // DrawBlend(window, "alpha_textures", 242, sf::Vector2f(0, 0), sf::BlendAdd);
        // window.setView(viewport);

        window.draw(mountainsSprite);

        // Draw the background shade at 0,0
        // backgroundShade.setPosition(viewport.getCenter().x - game_manager.getResolution().x / 2,
        //                             viewport.getCenter().y - game_manager.getResolution().y / 2);
        // backgroundShade.setFillColor(sf::Color(0, 0, 0, 50));
        // window.draw(backgroundShade);

        DrawBehindEntities(window, gameTime);
        DrawEntities(window, gameTime);
        DrawInFrontOfEntities(window, gameTime);
        // DEBUG_DrawMapBoundaries(window, gameTime);

        // Draw the UI
        window.setView(window.getDefaultView());
        player.health.Draw(window, gameTime);
}
void GameScene::InitializeScene() {
        float screenWidth = game_manager.getResolution().x;
        float screenHeight = game_manager.getResolution().y;
        viewport.setSize(sf::Vector2f(screenWidth, screenHeight)); // Set the view size to the window size TODO: Change this from hardcoded
        //viewport.setCenter(player.position); // Center the viewport on the player

        IScene::InitializeScene();
}
void GameScene::DestroyScene() {

}
void GameScene::OnScene_Active() {
        if (const auto debugOverlay = scene_manager.GetScene(SceneType::SCENE_DEBUG_OVERLAY)) {
                if (const auto debugOverlayPtr = std::dynamic_pointer_cast<DebugOverlay>(debugOverlay)) {
                        debugOverlayPtr->AddInfoBottomLeft("End Position Y", std::to_string(map->endPosition.getPosition().y));
                        debugOverlayPtr->AddInfoBottomLeft("End Position X", std::to_string(map->endPosition.getPosition().x));
                        debugOverlayPtr->AddInfoBottomLeft("Start Position Y", std::to_string(map->startPosition.y));
                        debugOverlayPtr->AddInfoBottomLeft("Start Position X", std::to_string(map->startPosition.x));
                        debugOverlayPtr->AddInfoBottomLeft("Map", mapName);
                        //debugOverlayPtr->AddInfoBottomLeft("Map", map->FileName);
                }
        }
}
void GameScene::OnScene_Deactivate() {
        if (const auto debugOverlay = scene_manager.GetScene(SceneType::SCENE_DEBUG_OVERLAY)) {
                if (const auto debugOverlayPtr = std::dynamic_pointer_cast<DebugOverlay>(debugOverlay)) {
                        debugOverlayPtr->RemoveInfoBottomLeft("Map");
                        debugOverlayPtr->RemoveInfoBottomLeft("Start Position X");
                        debugOverlayPtr->RemoveInfoBottomLeft("Start Position Y");
                        debugOverlayPtr->RemoveInfoBottomLeft("End Position X");
                        debugOverlayPtr->RemoveInfoBottomLeft("End Position Y");

                        debugOverlayPtr->RemoveInfoTopLeft("Player X");
                        debugOverlayPtr->RemoveInfoTopLeft("Player Y");
                        debugOverlayPtr->RemoveInfoTopLeft("Player Velocity X");
                        debugOverlayPtr->RemoveInfoTopLeft("Player Velocity Y");
                }
        }
}
void GameScene::DamagePlayer(int amount) {
        player.health.damage(amount);
}
void GameScene::Update_Loading(GameTime gameTime) {
        // HACK: ----------------------------------------------------
        // There is an issue with delta time while the map is loading
        // This caused the player to be launched down the map
        // This is a temporary fix to allow the game to load
        // TODO: Play a level start sequence to allow for delta time
        // to stabilize
        if (startTime == 0.f)
                startTime = gameTime.NowAddMilliseconds(1000);

        if (!gameTime.TimeElapsed(startTime))
                UpdateLoop = &GameScene::Update_Game;
        // END HACK: ------------------------------------------------
}
void GameScene::Update_Game(GameTime gameTime) {

        if (player.GetIsDead() && sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
                player.health.ResetHealth(gameTime);
                player.SetIsFalling(false);
                player.SetIsDead(false);
                SpawnPlayer();
        }

        // Update the map objects
        for (auto const & obj: map->LevelObjects)
                asset_manager.ObjectLibraries[obj.ObjectLibraryFile]->Update(gameTime);

        viewport.setCenter(player.position + sf::Vector2f(250,0)); // Center the viewport on the player to culling logic
        // Pass boundaries to Player and calculate the physics state
        player.CalculatePhysicsState(getLocalBoundaries(), gameTime);
        player.Update(gameTime);

        if (player.position.y > 6000)
                SpawnPlayer();
                //player.position = map->startPosition - sf::Vector2f(250, 0);

        // This is a possible fix to the tearing issue of the tiles
        int vpX = player.position.x + 250;
        int vpY = player.position.y + 250;
        viewport.setCenter(vpX, vpY); // Center the viewport on the player to int value
        //viewport.setCenter(player.position + sf::Vector2f(250,250)); // Center the viewport on the player

        // Check if player gets to the end of the level
        sf::IntRect endPosRect;
        endPosRect.left = map->endPosition.getPosition().x;
        endPosRect.top = map->endPosition.getPosition().y;
        endPosRect.width = map->endPosition.getSize().x;
        endPosRect.height = map->endPosition.getSize().y;

        sf::IntRect playerRect;
        playerRect.left = player.position.x + player.collisionBox.getPosition().x;
        playerRect.top = player.position.y + player.collisionBox.getPosition().y;
        playerRect.width = player.collisionBox.getSize().x;
        playerRect.height = player.collisionBox.getSize().y;

        if (playerRect.intersects(endPosRect)) {
                // TODO: Implement a level transition
                SpawnPlayer();
                scene_manager.ChangeScene(SceneType::SCENE_MAIN_MENU);
        }

        //UpdateMobs(gameTime);
        for (auto &monster: monsters) {
                monster->CalculatePhysicsState(getLocalBoundaries(), gameTime);
                //monster->UpdatePlayerPosition(player.position, gameTime);
                monster->Update(gameTime);
        }
}
void GameScene::ValidateMap() {
        // Check if the map exists in the asset manager
        if (asset_manager.Maps.contains(mapName)) {
                // Get the map from the asset manager
                map = asset_manager.Maps[mapName].get();
        } else {
                std::cerr << "Map " << mapName << " does not exist in the asset manager." << std::endl;
                SceneManager::GetInstance().ChangeScene(SceneType::SCENE_MAIN_MENU);
        }
}
void GameScene::LoadSky() {
        // Check if the ParallaxBackgroundIndex is valid
        if (map->ParallaxBackgroundIndex < 0 || map->ParallaxBackgroundIndex >= asset_manager.TextureLibraries["sky"]->entryCount)
                map->ParallaxBackgroundIndex = 0; // Default to 0 if invalid

        // Load the parallax background texture
        auto &skyLibrary = *asset_manager.TextureLibraries["sky"];
        skyLibrary.LoadIndices({map->ParallaxBackgroundIndex});
        skyBoxTexture = &skyLibrary.entries[map->ParallaxBackgroundIndex].texture;

        // Set the position of the skyBoxSprite to the top-left corner
        skyBoxSprite.setTexture(*skyBoxTexture, true);
        skyBoxSprite.setPosition(0, 0);
}
void GameScene::LoadMountains() {
        if (map->MountainsBackgroundIndex < 0 || map->MountainsBackgroundIndex >= asset_manager.TextureLibraries["mountains"]->entryCount) {
                map->MountainsBackgroundIndex = 0; // Default to 0 if invalid
            }

        // Load the mountains background texture
        auto &mountainsLibrary = *asset_manager.TextureLibraries["mountains"];
        mountainsLibrary.LoadIndices({map->MountainsBackgroundIndex});
        mountainsTexture = &mountainsLibrary.entries[map->MountainsBackgroundIndex].texture;

        // Set the texture for the mountains sprite
        mountainsSprite.setTexture(*mountainsTexture, true);
}
void GameScene::LoadAssets() {
        ValidateMap();
        LoadSky();
        LoadMountains();
        LoadMobs();

        // Load required assets
        // Map from texture library name to set of unique indices
        std::unordered_map<std::string, std::unordered_set<int>> libraryToIndices;

        for (const auto &wmObject: map->LevelObjects) {
                const auto &oLibraryName = wmObject.ObjectLibraryFile;

                // Skip if library doesn't exist
                if (!asset_manager.ObjectLibraries.contains(oLibraryName)) {
                        std::cerr << "Object library " << oLibraryName << " does not exist." << std::endl;
                        continue;
                }

                const auto &oLibrary = asset_manager.ObjectLibraries.at(oLibraryName);

                for (const auto &graphic: oLibrary->Images) {
                        // Skip graphics without an associated library
                        if (graphic.BackImageLibrary.empty()) {
                                std::cerr << "Graphic in library " << oLibraryName
                                          << " has no associated BackImageLibrary." << std::endl;
                                continue;
                        }

                        if (graphic.BackIndex < 0 ||
                            graphic.BackIndex >= asset_manager.TextureLibraries[graphic.BackImageLibrary]->entryCount) {
                                std::cerr << "Graphic in library " << oLibraryName << " has invalid BackIndex."
                                          << std::endl;
                                std::cerr << "Expected range: [0, " << oLibrary->Images.size() << "]" << std::endl;
                                std::cerr << "Actual BackIndex: " << graphic.BackIndex << std::endl;
                                continue;
                        }

                        if (graphic.BackEndIndex == -1) {
                                // If BackEndIndex is -1, only add BackIndex
                                libraryToIndices[graphic.BackImageLibrary].insert(graphic.BackIndex);
                        } else {
                                // Otherwise add the range from BackIndex to BackEndIndex
                                for (int idx = graphic.BackIndex; idx <= graphic.BackEndIndex; ++idx) {
                                        libraryToIndices[graphic.BackImageLibrary].insert(idx);
                                }
                        }
                }
        }

        // Load the indices for each texture library
        for (const auto &[libraryName, indicesSet]: libraryToIndices) {
                if (asset_manager.TextureLibraries.contains(libraryName)) {
                        std::vector<int> indices(indicesSet.begin(), indicesSet.end());
                        asset_manager.TextureLibraries[libraryName]->LoadIndices(indices);
                }
        }

        asset_manager.TextureLibraries["alpha_textures"]->LoadIndices({242}); // Stars
}
void GameScene::LoadMobs() {
        // Clear exisitng mobs
        monsters.clear();

        for (const auto &mob: map->Mobs) {
                // Check if the mob library exists
                if (!asset_manager.TextureLibraries.contains(mob.MonsterName)) {
                        std::cerr << "Mob library " << mob.MonsterName << " does not exist." << std::endl;
                        continue;
                }
                asset_manager.TextureLibraries[mob.MonsterName]->LoadIndices(
                                {}); // Load all indices for the mob library

                if (mob.MonsterName == "ChestMonster") {
                        monsters.emplace_back(std::make_unique<ChestMonster>(&player, mob.Position, mob.ViewRange, mob.MoveSpeed, mob.Health));
                }
                // else if (mob.MonsterName == "SlimeMonster") {
                //     slimeMonsters.emplace_back(mob.Position, mob.ViewRange, mob.MoveSpeed, mob.Health);
                // }
                else {
                        std::cerr << "Unknown mob type: " << mob.MonsterName << std::endl;
                }
        }
}
// IDE Says this is unreachable, but it is.
void GameScene::UpdateMobs(GameTime gameTime) {

}
// TODO: Split the sky and mountain
void GameScene::CalculateParallaxBackground() {
        // Get world dimensions
        float worldWidth = map->endPosition.getPosition().x + 1000.f;
        float worldHeight = 10000.0f;

        // Get screen dimensions
        const auto screenSize = game_manager.getResolution();
        const float screenWidth = static_cast<float>(screenSize.x);
        const float screenHeight = static_cast<float>(screenSize.y);

        // Get player's position as a normalized value (0 - 1)
        const sf::Vector2f center = viewport.getCenter();
        const float normalX = std::clamp(center.x / worldWidth, 0.0f, 1.0f);
        const float normalY = std::clamp(center.y / worldHeight, 0.0f, 1.0f);

        // Parallax factors
        constexpr float skyParallax = 0.1f;
        constexpr float mountainsParallax = 1.f;

        // Calculate offsets
        const sf::Vector2u skySize = skyBoxTexture->getSize();
        const sf::Vector2u mountainsSize = mountainsTexture->getSize();
        const int skyX = static_cast<int>((skySize.x - screenWidth) * normalX * skyParallax);
        const int skyY = static_cast<int>((skySize.y - screenHeight) * normalY * skyParallax);
        const int mountainsX = static_cast<int>((mountainsSize.x - screenWidth) * normalX * mountainsParallax);
        const int mountainsY = static_cast<int>((mountainsSize.y - screenHeight) * normalY * mountainsParallax);

        // Set texture rects and positions
        skyBoxSprite.setTextureRect(sf::IntRect(skyX, skyY, static_cast<int>(screenWidth), static_cast<int>(screenHeight)));
        skyBoxSprite.setPosition(center.x - screenWidth / 2, center.y - screenHeight / 2);

        mountainsSprite.setTextureRect(sf::IntRect(mountainsX, mountainsY, static_cast<int>(screenWidth), static_cast<int>(screenHeight)));
        mountainsSprite.setPosition(center.x - screenWidth / 2, center.y - screenHeight / 2);
}
void GameScene::DrawBehindEntities(sf::RenderWindow &window, GameTime gameTime) {
        for (int layer = 0; layer <= 3; ++layer) {
                // if (layer == 2) {
                //         // Draw the background shade at 0,0
                //         backgroundShade.setPosition(viewport.getCenter().x - game_manager.getResolution().x / 2,
                //                                     viewport.getCenter().y - game_manager.getResolution().y / 2);
                //         backgroundShade.setFillColor(sf::Color(0, 0, 0, 100));
                //         window.draw(backgroundShade);
                // }

                for (auto const & obj: map->LevelObjects)
                        for (auto const &entry: asset_manager.ObjectLibraries[obj.ObjectLibraryFile]->Images)
                                if (entry.DrawLayer == layer)
                                        IDraw::Draw(window, entry.BackImageLibrary, entry.currentFrame, sf::Vector2f(obj.Position.x, obj.Position.y));
        }
}
void GameScene::DrawEntities(sf::RenderWindow &window, GameTime gameTime) {
        player.Draw(window, gameTime);
        for (auto const & chestMonster : monsters) {
                chestMonster->Draw(window, gameTime);
        }
}
void GameScene::DrawInFrontOfEntities(sf::RenderWindow &window, GameTime gameTime) {
        for (int layer = 5; layer <= 7; ++layer)
                for (auto const &obj: map->LevelObjects)
                        for (auto const &entry: asset_manager.ObjectLibraries[obj.ObjectLibraryFile]->Images)
                                if (entry.DrawLayer == layer)
                                        IDraw::Draw(window, entry.BackImageLibrary, entry.currentFrame,
                                                    sf::Vector2f(obj.Position.x, obj.Position.y));
}
void GameScene::DEBUG_DrawMapBoundaries(sf::RenderWindow &window, GameTime gameTime) {
        auto localBoundaries = getLocalBoundaries();
        for (auto const &boundary: localBoundaries) {
                sf::RectangleShape rect(sf::Vector2f(boundary.Width, boundary.Height));
                rect.setPosition(boundary.X, boundary.Y);
                rect.setFillColor(sf::Color(255, 0, 0, 128));
                rect.setOutlineColor(sf::Color::Red);
                rect.setOutlineThickness(1.0f);
                window.draw(rect);
        }

}
//Function to get all boundaries in the current viewport
std::vector<Boundary> GameScene::getLocalBoundaries() const {
        std::vector<Boundary> localBoundaries;
        localBoundaries.reserve(map->LevelObjects.size());

        const sf::FloatRect viewBounds(viewport.getCenter().x - viewport.getSize().x / 2,
                                       viewport.getCenter().y - viewport.getSize().y / 2, viewport.getSize().x,
                                       viewport.getSize().y);

        for (const auto &obj: map->LevelObjects) {
                const auto &objLibIt = asset_manager.ObjectLibraries.find(obj.ObjectLibraryFile);
                if (objLibIt == asset_manager.ObjectLibraries.end())
                        continue;
                const auto &images = objLibIt->second->Images;

                for (const auto &entry: images) {
                        if (!entry.Boundaries || entry.Boundaries->empty())
                                continue;

                        const int currentFrame = entry.currentFrame - entry.BackIndex;
                        if (currentFrame < 0 || currentFrame >= static_cast<int>(entry.Boundaries->size()))
                                continue;

                        const Boundary &boundary = entry.Boundaries->at(currentFrame);

                        if (!boundary.Active)
                                continue;

                        sf::FloatRect boundaryRect(obj.Position.x + boundary.X, obj.Position.y + boundary.Y,
                                                   boundary.Width, boundary.Height);

                        // TODO: Maybe just use FloatRec since its already created
                        // Or maybe implement a check within the boundary class
                        // NOTE: Probally not gonna do it unless its a performance issue
                        if (viewBounds.intersects(boundaryRect)) {
                                // Create a copy with adjusted position
                                Boundary adjustedBoundary = boundary;
                                adjustedBoundary.X = obj.Position.x + boundary.X;
                                adjustedBoundary.Y = obj.Position.y + boundary.Y;
                                localBoundaries.push_back(adjustedBoundary);
                        }
                }
        }
        return localBoundaries;
}
void GameScene::SpawnPlayer() {
        player.position = map->startPosition - sf::Vector2f(250, 0);
        player.velocity = sf::Vector2f(0, 0);
        player.acceleration = sf::Vector2f(0, 0);
        player.deceleration = sf::Vector2f(0, 0);
        viewport.setCenter(player.position + sf::Vector2f(250,0));
}
