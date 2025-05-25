//
// Created by Paul McGinley on 24/04/2025.
//

#include "GameScene.h"

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include "LoadingScene.h"
#include "Overlays/DebugOverlay.h"
#include "managers/SceneManager.h"
#include "models/LevelObject/Collectable.h"
#include "models/LevelObject/OLibrary.h"
#include "models/MapObject/WMap.h"
#include "objects/mobs/AngryMushroom.h"
#include "objects/mobs/Cactus.h"

#include "objects/mobs/EyeBall.h"
#include "objects/mobs/HappyMushroom.h"
#include "objects/mobs/SlimeMonster.h"

GameScene::GameScene()
        : IScene()
        , UpdateLoop(&GameScene::Update_Loading)
        , map(nullptr) { /* Nothing in the constructor */ }

void GameScene::LoadMap(std::string name)  {
        mapName = std::move(name);
        LoadAssets();
        SpawnPlayer();

        if (const auto debugOverlay = sceneManager.GetScene(SceneType::SCENE_DEBUG_OVERLAY)) {
                if (const auto debugOverlayPtr = std::dynamic_pointer_cast<DebugOverlay>(debugOverlay)) {
                        debugOverlayPtr->AddInfoBottomLeft("Collectables", std::to_string(collectables.size()));
                }
        }
}

void GameScene::Update(const GameTime gameTime) {
        (this->*UpdateLoop)(gameTime);

        // DEBUG:
        if (const auto debugOverlay = sceneManager.GetScene(SceneType::SCENE_DEBUG_OVERLAY)) {
                if (const auto debugOverlayPtr = std::dynamic_pointer_cast<DebugOverlay>(debugOverlay)) {
                        debugOverlayPtr->AddInfoTopLeft("Player X", std::to_string(player.position.x));
                        debugOverlayPtr->AddInfoTopLeft("Player Y", std::to_string(player.position.y));
                        debugOverlayPtr->AddInfoTopLeft("Player Velocity X", std::to_string(player.velocity.x));
                        debugOverlayPtr->AddInfoTopLeft("Player Velocity Y", std::to_string(player.velocity.y));
                        debugOverlayPtr->AddInfoTopLeft("Coins", std::to_string(player.GetCoins()));
                        debugOverlayPtr->AddInfoTopLeft("Score", std::to_string(player.GetTotalScore()));
                        debugOverlayPtr->AddInfoTopLeft("Time", levelTime(gameTime, true));
                }
        }
        // END DEBUG ^
}

void GameScene::LateUpdate(const GameTime gameTime) {
        bigCoin.UpdatePercent(LevelScorePercent());
        bigCoin.LateUpdate(gameTime);

        // Call LateUpdate for each projectile
        for (const auto& projectile : projectiles) {
                projectile->LateUpdate(gameTime);
        }

        player.LateUpdate(gameTime);

        // Call LateUpdate for each Mobj
        for (auto const & mob: monsters) {
                mob->LateUpdate(gameTime);
        }

        for (auto const & collectable: collectables) {
                collectable->LateUpdate(gameTime);
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

        // Void
        window.draw(deadLineSprite);
        window.draw(blackness);

        DrawEntities(window, gameTime);
        DrawInFrontOfEntities(window, gameTime);

        bigCoin.Draw(window, gameTime);

        if(gameManager.ShowCollisions()) {
                DEBUG_DrawMapBoundaries(window, gameTime);
        }

        // Draw the UI
        window.setView(window.getDefaultView());
        player.health.Draw(window, gameTime);

        window.draw(timerText);

        if (UpdateLoop == &GameScene::Update_EndOfLevel)
                summaryOverlay->Draw(window, gameTime);
}
void GameScene::InitializeScene() {
        float screenWidth = gameManager.getResolution().x;
        float screenHeight = gameManager.getResolution().y;
        viewport.setSize(sf::Vector2f(screenWidth, screenHeight)); // Set the view size to the window size
        //viewport.setCenter(player.position); // Center the viewport on the player

        timerFont = *assetManager.Fonts["OpenSans-Bold"].get();
        timerText.setFont(timerFont);
        timerText.setString("00:00");
        timerText.setFillColor(sf::Color(255,255,255,128));
        timerText.setCharacterSize(50);
        timerText.setStyle(sf::Text::Bold);
        timerText.setOutlineColor(sf::Color::Black);
        timerText.setOutlineThickness(2);

        timerText.setPosition((screenWidth/2)- (timerText.getGlobalBounds().width/2), 20);

        IScene::InitializeScene();
}
void GameScene::DestroyScene() { /* Nothing to destroy */ }
void GameScene::OnScene_Activate() {
        // Init void
        deadLine = assetManager.TextureLibraries["PrgUse"]->entries[2].texture;
        deadLineSprite.setTexture(deadLine);
        deadLineSprite.setPosition(-5000, 6000);
        deadLineSprite.setScale(10000, 1.0f);
        blackness.setSize(sf::Vector2f(105000, 3500));
        blackness.setPosition(-5000, 6500);
        blackness.setFillColor(sf::Color::Black);

        player.visible = true;
        bigCoin.visible = true;
        summaryOverlay = std::dynamic_pointer_cast<EndOfLevel>(sceneManager.GetScene(SceneType::SCENE_END_OF_LEVEL));
        UpdateLoop = &GameScene::Update_Loading;

        if (!map->song.empty()) {
                assetManager.SetMusicVolume(map->song, 100.f);
                assetManager.PlayMusic(map->song, true);
        }

        std::cout << "Playing song: " << map->song << std::endl;

        if (const auto debugOverlay = sceneManager.GetScene(SceneType::SCENE_DEBUG_OVERLAY)) {
                if (const auto debugOverlayPtr = std::dynamic_pointer_cast<DebugOverlay>(debugOverlay)) {
                        debugOverlayPtr->AddInfoBottomLeft("End Position Y", std::to_string(map->endPosition.getPosition().y));
                        debugOverlayPtr->AddInfoBottomLeft("End Position X", std::to_string(map->endPosition.getPosition().x));
                        debugOverlayPtr->AddInfoBottomLeft("Start Position Y", std::to_string(map->startPosition.y));
                        debugOverlayPtr->AddInfoBottomLeft("Start Position X", std::to_string(map->startPosition.x));
                        debugOverlayPtr->AddInfoBottomLeft("Map", mapName);
                }
        }
}
void GameScene::OnScene_Deactivate() {
        if (const auto debugOverlay = sceneManager.GetScene(SceneType::SCENE_DEBUG_OVERLAY)) {
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
        player.health.Damage(amount);
        player.ChangeAnimation(AnimationType::ANIMATION_DAMAGED, true);
}
void GameScene::Update_Loading(GameTime gameTime) {
        // HACK: ----------------------------------------------------
        // There is an issue with delta time while the map is loading
        // This caused the player to be launched down the map
        // This is a temporary fix to allow the game to load
        // TODO: Play a level start sequence to allow for delta time to stabilize
        // Note: This issue should now be resolved with the advent of the LoadingScene

        if (startTime == 0.f)
                startTime = gameTime.NowAddMilliseconds(0);

        if (!gameTime.TimeElapsed(startTime)) {
                UpdateLoop = &GameScene::Update_Game;

                levelStartTime = gameTime.NowAddMilliseconds(0);
        }
        // END HACK: ------------------------------------------------
}
void GameScene::Update_Game(GameTime gameTime) {

        timerText.setString(levelTime(gameTime, false));
        bigCoin.Update(gameTime);


        // Draw the collectables
        for (auto const &collectable: collectables) {
                collectable->Update(gameTime);
        }

        // Update projectiles
        for (const auto &projectile: projectiles) {
                projectile->Update(gameTime);
        }

        // Loop through the projectiles and remove any that have expired (start at the end to avoid invalidating
        // iterator)
        for (int i = projectiles.size() - 1; i >= 0; --i) {
                if (projectiles[i]->Expired()) {
                        // Remove the projectile from the list
                        projectiles.erase(projectiles.begin() + i);
                }
        }
        CheckProjectileCollisions(gameTime);

        if (player.GetIsDead() && sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
                player.health.ResetHealth(gameTime);
                player.SetIsFalling(false);
                player.SetIsDead(false);
                SpawnPlayer();
        }

        // Update the map objects
        for (auto const &obj: map->LevelObjects)
                assetManager.ObjectLibraries[obj.ObjectLibraryFile]->Update(gameTime);

        viewport.setCenter(player.position +
                           sf::Vector2f(250, 0)); // Center the viewport on the player to culling logic
        // Pass boundaries to Player and calculate the physics state
        player.CalculatePhysicsState(getLocalBoundaries(), gameTime);
        player.Update(gameTime);

        if (player.position.y > 6500)
                SpawnPlayer();

        // This is a possible fix to the tearing issue of the tiles
        int vpX = player.position.x + 250;
        int vpY = player.position.y + 250;
        viewport.setCenter(vpX, vpY); // Center the viewport on the player to int value
        // viewport.setCenter(player.position + sf::Vector2f(250,250)); // Center the viewport on the player

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
                levelEndTime = gameTime.NowAddMilliseconds(0) - levelStartTime;
                std::cout << mapName << ": Level End Time: " << levelEndTime << std::endl;
                startTime = 0.f;
                UpdateLoop = &GameScene::Update_Loading;

                summaryOverlay->ResetBoard();
                summaryOverlay->SetMapName(mapName);
                player.visible=false;
                bigCoin.visible=false;
                UpdateLoop = &GameScene::Update_EndOfLevel;


                // TODO: Update the summary overlay with the level data

                // std::vector<std::string> maps = {"00", "01", "02", "03"};
                // auto it = std::find(maps.begin(), maps.end(), mapName);
                // size_t nextIndex = 0;
                // if (it != maps.end()) {
                //         nextIndex = (std::distance(maps.begin(), it) + 1) % maps.size();
                // }
                // std::string nextMap = maps[nextIndex];
                //
                // auto scenePtr = sceneManager.GetScene(SceneType::SCENE_LOADER);
                // auto gameScene = std::dynamic_pointer_cast<LoadingScene>(scenePtr);
                // if (gameScene) {
                //         gameScene->BuildAssetQueue(nextMap);
                // }
                // sceneManager.ChangeScene(SceneType::SCENE_LOADER);


                // // TODO: Implement a level transition
                // SpawnPlayer();
                // scene_manager.ChangeScene(SceneType::SCENE_MAIN_MENU);
        }

        for (auto &monster: monsters) {
                if (monster->IsDead()) {
                        player.UpdateScore(mapName, monster->AwardScore());
                }

                monster->CalculatePhysicsState(getLocalBoundaries(), gameTime);
                monster->Update(gameTime);
        }

        for (auto &collectable: collectables) {
                if (collectable->IsCollected())
                        continue;

                sf::IntRect collider = player.collisionBox;
                collider.left += player.position.x;
                collider.top += player.position.y;
                if (collider.intersects(sf::Rect<int>(collectable->GetCollisionBox()))) {
                        collectable->Collect();
                        player.IncrementCoins(1);
                        player.UpdateScore(mapName, 100);
                }
        }

        for (int i = collectables.size() - 1; i >= 0; --i) {
                if (collectables[i]->IsCollected()) {
                        collectables.erase(collectables.begin() + i);
                }
        }
}
void GameScene::Update_EndOfLevel(GameTime gameTime) {
        summaryOverlay->Update(gameTime);
}
void GameScene::ValidateMap() {
        projectiles.clear();

        // Check if the map exists in the asset manager
        if (assetManager.Maps.contains(mapName)) {
                // Get the map from the asset manager
                map = assetManager.Maps[mapName].get();
                bigCoin.SetPosition({map->endPosition.getPosition().x + map->endPosition.getSize().x / 2,
                                    map->endPosition.getPosition().y + map->endPosition.getSize().y / 2});
        } else {
                std::cerr << "Map " << mapName << " does not exist in the asset manager." << std::endl;
                SceneManager::GetInstance().ChangeScene(SceneType::SCENE_MAIN_MENU);
        }
}
void GameScene::LoadSky() {
        // Load the parallax background texture
        const auto &skyLibrary = *assetManager.TextureLibraries["sky"];
        skyBoxTexture = &skyLibrary.entries[map->ParallaxBackgroundIndex].texture;

        // Set the position of the skyBoxSprite to the top-left corner
        skyBoxSprite.setTexture(*skyBoxTexture, true);
        skyBoxSprite.setPosition(0, 0);
}
void GameScene::LoadMountains() {
        // Load the mountains background texture
        const auto &mountainsLibrary = *assetManager.TextureLibraries["mountains"];
        mountainsTexture = &mountainsLibrary.entries[map->MountainsBackgroundIndex].texture;

        // Set the texture for the mountains sprite
        mountainsSprite.setTexture(*mountainsTexture, true);
        mountainsSprite.setPosition(0, 0);
}
void GameScene::LoadAssets() {
        ValidateMap();
        LoadSky();
        LoadMountains();
        LoadMobs();
        LoadCollectables();
}
void GameScene::LoadMobs() {
        // Clear existing mobs
        monsters.clear();

        std::cout << "Loading mob count: " << map->Mobs.size() << std::endl;
        for (const auto &mob: map->Mobs) {
                std::cout << "Loading mob: " << mob.MonsterName << std::endl;
                // Check if the mob library exists
                if (!assetManager.TextureLibraries.contains(mob.MonsterName)) {
                        std::cerr << "Mob library " << mob.MonsterName << " does not exist." << std::endl;
                        continue;
                }
                if (!assetManager.TextureLibraries[mob.MonsterName]->fullyLoaded) {
                        assetManager.TextureLibraries[mob.MonsterName]->LoadIndices(
                                        {}); // Load all indices for the mob library
                }

                if (mob.MonsterName == "ChestMonster") {
                        monsters.emplace_back(std::make_unique<ChestMonster>(&player, mob.Position, mob.ViewRange, mob.MoveSpeed, mob.Health));
                } else if (mob.MonsterName == "Eye-Ball") {
                        monsters.emplace_back(std::make_unique<EyeBall>(&player, mob.Position, mob.ViewRange, mob.MoveSpeed, mob.Health));
                }  else if (mob.MonsterName == "HappyMushroom") {
                        monsters.emplace_back(std::make_unique<HappyMushroom>(&player, mob.Position, mob.ViewRange, mob.MoveSpeed, mob.Health));
                } else if (mob.MonsterName == "SlimeMonster") {
                        monsters.emplace_back(std::make_unique<SlimeMonster>(&player, mob.Position, mob.ViewRange, mob.MoveSpeed, mob.Health));
                } else if (mob.MonsterName == "Cactus") {
                        monsters.emplace_back(std::make_unique<Cactus>(&player, mob.Position, mob.ViewRange, mob.MoveSpeed, mob.Health));
                } else if (mob.MonsterName == "AngryMushroom") {
                        monsters.emplace_back(std::make_unique<AngryMushroom>(&player, mob.Position, mob.ViewRange, mob.MoveSpeed, mob.Health));
                } else {
                        std::cerr << "Unknown mob type: " << mob.MonsterName << std::endl;
                }
        }
}
void GameScene::LoadCollectables() {
        collectables.clear();

        std::cout << "Loading collectable count: " << map->Collectables.size() << std::endl;
        for (const auto &collectable : map->Collectables) {
                // Check if the collectable library exists
                if (!assetManager.Collectables.contains(collectable.CollectableName)) {
                        std::cerr << "Collectable library " << collectable.CollectableName << " does not exist." << std::endl;
                        continue;
                }

                if (!assetManager.Collectables[collectable.CollectableName].empty()) {
                        // Create a new Collectable by copying the template one
                        auto newCollectable = std::make_unique<Collectable>(
                            *assetManager.Collectables[collectable.CollectableName].back().get());

                        // Set the position
                        newCollectable->SetPosition(collectable.Position.x, collectable.Position.y);

                        // Add to our collection
                        collectables.push_back(std::move(newCollectable));
                }
        }
}
void GameScene::CheckProjectileCollisions(GameTime gameTime) {
        for (const auto &projectile: projectiles) {
                for (const auto &mob: monsters) {
                        if (projectile->Intersects(mob->GetCollisionBox())) {
                                // Calculate the collision point to display the explosion
                                sf::Vector2f collisionPoint;
                                collisionPoint.x = mob->GetCollisionBox().left + mob->GetCollisionBox().width / 2;
                                collisionPoint.y = mob->GetCollisionBox().top + mob->GetCollisionBox().height / 2;

                                // TODO: Implement the mob damage
                                int damage = projectile->Collide(collisionPoint);
                                mob->Damaged(damage, gameTime);
                        }
                }
        }
}
// TODO: Split the sky and mountain
void GameScene::CalculateParallaxBackground() {
        // Get world dimensions
        float worldWidth = map->endPosition.getPosition().x + 1000.f;
        float worldHeight = 10000.0f;

        // Get screen dimensions
        const auto screenSize = gameManager.getResolution();
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
                if (layer == 2) {
                        // Draw the background shade at 0,0
                        backgroundShade.setPosition(viewport.getCenter().x - gameManager.getResolution().x / 2,
                                                    viewport.getCenter().y - gameManager.getResolution().y / 2);
                        backgroundShade.setFillColor(sf::Color(0, 0, 0, 100));
                        window.draw(backgroundShade);
                }

                for (auto const & obj: map->LevelObjects)
                        for (auto const &entry: assetManager.ObjectLibraries[obj.ObjectLibraryFile]->Images)
                                if (entry.DrawLayer == layer)
                                        IDraw::Draw(window, entry.BackImageLibrary, entry.currentFrame, sf::Vector2f(obj.Position.x, obj.Position.y));
        }
}
void GameScene::DrawEntities(sf::RenderWindow &window, const GameTime gameTime) {
        // Draw dead monsters behind the player
        for (auto const & monster : monsters) {
                if (monster->IsDead())
                        monster->Draw(window, gameTime);
        }

        player.Draw(window, gameTime);

        // Draw alive monsters in front of the player
        for (auto const & monster : monsters) {
                if (!monster->IsDead())
                        monster->Draw(window, gameTime);
        }
        for (const auto& projectile : projectiles) {
                projectile->Draw(window, gameTime);
        }
        // Draw the collectables
        for (auto const & collectable: collectables) {
                collectable->Draw(window, gameTime);
        }

}
void GameScene::DrawInFrontOfEntities(sf::RenderWindow &window, GameTime gameTime) {
        for (int layer = 5; layer <= 7; ++layer)
                for (auto const &obj: map->LevelObjects)
                        for (auto const &entry: assetManager.ObjectLibraries[obj.ObjectLibraryFile]->Images)
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
                const auto &objLibIt = assetManager.ObjectLibraries.find(obj.ObjectLibraryFile);
                if (objLibIt == assetManager.ObjectLibraries.end())
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
        viewport.setCenter(player.position + sf::Vector2f(250, 0));
}
std::string GameScene::levelTime(GameTime gameTime, bool withMilliseconds = true) {
        float durationMs = (gameTime.totalGameTime - levelStartTime) * 1000.f; // Convert to milliseconds

        int totalMilliseconds = static_cast<int>(durationMs);
        int minutes = totalMilliseconds / (60 * 1000);
        totalMilliseconds %= (60 * 1000);
        int seconds = totalMilliseconds / 1000;
        int milliseconds = totalMilliseconds % 1000;

        std::ostringstream oss;
        oss << std::setfill('0') << std::setw(2) << minutes << ":" << std::setfill('0') << std::setw(2) << seconds;

        if (withMilliseconds)
                oss << "." << std::setfill('0') << std::setw(3) << milliseconds;

        return oss.str();
}
float GameScene::LevelScorePercent() {
        int totalCollectables = map->Collectables.size();
        int remainingCollectables = collectables.size();

        int totalMonsters = map->Mobs.size();
        int remainingMonsters = std::count_if(monsters.begin(), monsters.end(),
                [](const std::unique_ptr<Mob>& mob) { return !mob->IsDead(); });

        // Calculate collected coins percentage (avoid division by zero)
        float collectablesPercent = (totalCollectables > 0) ?
            100.0f * (totalCollectables - remainingCollectables) / totalCollectables : 0.0f;

        // Calculate killed monsters percentage (avoid division by zero)
        float monstersPercent = (totalMonsters > 0) ?
            100.0f * (totalMonsters - remainingMonsters) / totalMonsters : 0.0f;

        // Average the two percentages
        float overallPercent = (collectablesPercent + monstersPercent) / 2.0f;

        return overallPercent;
}