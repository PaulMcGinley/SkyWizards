//
// Created by Paul McGinley on 25/01/2025.
//

#include "MainMenu.h"

#include "GameScene.h"
#include "LoadingScene.h"
#include "managers/InputManager.h"
#include "managers/SceneManager.h"

MainMenu::MainMenu()
        :       selectedMenuIndex(0) { /* Nothing in the constructor */ }
void MainMenu::Update(GameTime gameTime) {
        InputManager &input = InputManager::getInstance();

        bool selectionChanged = false;

        // Inspired from: https://dev.to/avocoaster/how-to-wrap-around-a-range-of-numbers-with-the-modulo-cdo
        // Handle menu navigation
        if (input.IsKeyPressed(sf::Keyboard::Key::Down) || input.IsKeyPressed(sf::Keyboard::Key::S)) {
                selectedMenuIndex = (selectedMenuIndex + 1) % 5;
                selectionChanged = true;
        } else if (input.IsKeyPressed(sf::Keyboard::Key::Up) || input.IsKeyPressed(sf::Keyboard::Key::W)) {
                selectedMenuIndex = (selectedMenuIndex + 4) % 5;
                selectionChanged = true;
        }

        // If the selection has changed, update the menu item textures
        if (selectionChanged)
                UpdateMenuSelection();

        // Handle confimation of selected item
        if (input.IsKeyPressed(sf::Keyboard::Key::Space) || input.IsKeyPressed(sf::Keyboard::Key::Num4)) {
                switch (selectedMenuIndex) {
                        case 0: { // Play
                                auto scenePtr = sceneManager.GetScene(SceneType::SCENE_LOADER);
                                auto gameScene = std::dynamic_pointer_cast<LoadingScene>(scenePtr);
                                if (gameScene) {
                                        gameScene->BuildAssetQueue("Lv_01");
                                }
                                sceneManager.ChangeScene(SceneType::SCENE_LOADER);
                                break;
                        }
                        case 1: { // Level Select
                                // ...
                                break;
                        }
                        case 2: { // Settings
                                sceneManager.ChangeScene(SceneType::SCENE_OPTIONS);
                                break;
                        }
                        case 3: { // Rankings
                                // ...
                                break;
                        }
                        case 4: { // Leave
                                gameManager.window->close();
                                break;
                        }
                }
        }

        float t = gameTime.totalGameTime; // time in seconds
        float amplitude = 0.2f; // scale amplitude
        float speed = 2.0f; // speed of animation
        float offset = 0.7f; // offset for the second sprite (so they don't scale the same)

        float scale1 = 1.0f + amplitude * std::sin(t * speed + (offset * 2));
        float scale2 = 1.0f + amplitude * std::sin(t * speed + offset);
        float scale3 = 1.0f + amplitude * std::sin(t * speed);

        title1Sprite.setScale(scale1, scale1);
        title2Sprite.setScale(scale2, scale2);
        title3Sprite.setScale(scale3, scale3);

       // menuPlaySprite.setScale((scale1 + scale2 + scale3) / 3, (scale1 + scale2 + scale3) / 3);
}
void MainMenu::LateUpdate(GameTime gameTime) {}
void MainMenu::Draw(sf::RenderWindow &window, GameTime gameTime) {
        window.draw(backgroundQuad, &assetManager.TextureLibraries["PrgUse"].get()->entries[0].texture);

        window.draw(title1Sprite);
        window.draw(title2Sprite);
        window.draw(title3Sprite);

        window.draw(menuPlaySprite);
        window.draw(menuLevelSelectSprite);
        window.draw(menuSettingsSprite);
        window.draw(menuRankingsSprite);
        window.draw(menuLeaveSprite);
}
void MainMenu::InitializeScene() {
        float screenWidth = gameManager.getResolution().x;
        float screenHeight = gameManager.getResolution().y;

        // Set up the background quad to cover the entire window
        backgroundQuad[0].position = sf::Vector2f(0, 0);
        backgroundQuad[1].position = sf::Vector2f(screenWidth, 0);
        backgroundQuad[2].position = sf::Vector2f(screenWidth, screenHeight);
        backgroundQuad[3].position = sf::Vector2f(0, screenHeight);

        backgroundQuad[0].texCoords = sf::Vector2f(0, 0);
        backgroundQuad[1].texCoords = sf::Vector2f(screenWidth, 0);
        backgroundQuad[2].texCoords = sf::Vector2f(screenWidth, screenHeight);
        backgroundQuad[3].texCoords = sf::Vector2f(0, screenHeight);

        IScene::InitializeScene(); // Call the parent class to set the scene as initialized
}
void MainMenu::DestroyScene() {}
void MainMenu::OnScene_Activate() {
        float screenWidth = gameManager.getResolution().x;
        float screenHeight = gameManager.getResolution().y;

        // TODO: Move this to the TextureLibraries map
        //title1Texture = &asset_manager.TextureLibraries["PrgUse"]->entries[7].texture;
        title1Texture = &assetManager.TextureLibraries["PrgUse"].get()->entries[7].texture;
        title2Texture = &assetManager.TextureLibraries["PrgUse"].get()->entries[6].texture;
        title3Texture = &assetManager.TextureLibraries["PrgUse"].get()->entries[5].texture;

        title1Sprite.setTexture(*title1Texture, true);
        title2Sprite.setTexture(*title2Texture, true);
        title3Sprite.setTexture(*title3Texture, true);

        title1Sprite.setOrigin(title1Texture->getSize().x / 2, title1Texture->getSize().y / 2);
        title2Sprite.setOrigin(title2Texture->getSize().x / 2, title2Texture->getSize().y / 2);
        title3Sprite.setOrigin(title3Texture->getSize().x / 2, title3Texture->getSize().y / 2);

        title1Sprite.setPosition(screenWidth/2, (screenHeight/2) - 150 - 200);
        title2Sprite.setPosition(screenWidth/2, (screenHeight/2) -200);
        title3Sprite.setPosition(screenWidth/2, (screenHeight/2) + 130 - 200);

        // Set smooth textures to prevent pixelation (Bilinear filtering)
        title1Texture->setSmooth(true);
        title2Texture->setSmooth(true);
        title3Texture->setSmooth(true);


        // Initialize menu item textures
        menuPlayTextures[0] = &assetManager.TextureLibraries["PrgUse"].get()->entries[10].texture;
        menuPlayTextures[1] = &assetManager.TextureLibraries["PrgUse"].get()->entries[11].texture;
        menuLevelSelectTextures[0] = &assetManager.TextureLibraries["PrgUse"].get()->entries[12].texture;
        menuLevelSelectTextures[1] = &assetManager.TextureLibraries["PrgUse"].get()->entries[13].texture;
        menuSettingsTextures[0] = &assetManager.TextureLibraries["PrgUse"].get()->entries[14].texture;
        menuSettingsTextures[1] = &assetManager.TextureLibraries["PrgUse"].get()->entries[15].texture;
        menuRankingsTextures[0] = &assetManager.TextureLibraries["PrgUse"].get()->entries[16].texture;
        menuRankingsTextures[1] = &assetManager.TextureLibraries["PrgUse"].get()->entries[17].texture;
        menuLeaveTextures[0] = &assetManager.TextureLibraries["PrgUse"].get()->entries[18].texture;
        menuLeaveTextures[1] = &assetManager.TextureLibraries["PrgUse"].get()->entries[19].texture;

        menuPlayTextures[0]->setSmooth(true);
        menuPlayTextures[1]->setSmooth(true);
        menuLevelSelectTextures[0]->setSmooth(true);
        menuLevelSelectTextures[1]->setSmooth(true);
        menuSettingsTextures[0]->setSmooth(true);
        menuSettingsTextures[1]->setSmooth(true);
        menuRankingsTextures[0]->setSmooth(true);
        menuRankingsTextures[1]->setSmooth(true);
        menuLeaveTextures[0]->setSmooth(true);
        menuLeaveTextures[1]->setSmooth(true);

        menuPlaySprite.setTexture(*menuPlayTextures[0], true);
        menuLevelSelectSprite.setTexture(*menuLevelSelectTextures[0], true);
        menuSettingsSprite.setTexture(*menuSettingsTextures[0], true);
        menuRankingsSprite.setTexture(*menuRankingsTextures[0], true);
        menuLeaveSprite.setTexture(*menuLeaveTextures[0], true);

        // Set initial positions for menu sprites
        menuPlaySprite.setPosition(screenWidth/2, (screenHeight/2) +100);
        menuLevelSelectSprite.setPosition(screenWidth/2, (screenHeight/2) + 190);
        menuSettingsSprite.setPosition(screenWidth/2, (screenHeight/2) + 280);
        menuRankingsSprite.setPosition(screenWidth/2, (screenHeight/2) + 370);
        menuLeaveSprite.setPosition(screenWidth/2, (screenHeight/2) + 460);

        // Set origin for menu sprites to center them
        for (auto& sprite : {&menuPlaySprite, &menuLevelSelectSprite, &menuSettingsSprite, &menuRankingsSprite, &menuLeaveSprite}) {
                sprite->setOrigin(sprite->getTexture()->getSize().x / 2, sprite->getTexture()->getSize().y / 2);
        }

        UpdateMenuSelection();
}
void MainMenu::OnScene_Deactivate() { selectedMenuIndex = 0; }
void MainMenu::UpdateMenuSelection() {
        menuPlaySprite.setTexture(*(selectedMenuIndex == 0 ? menuPlayTextures[1] : menuPlayTextures[0]), true);
        menuLevelSelectSprite.setTexture(*(selectedMenuIndex == 1 ? menuLevelSelectTextures[1] : menuLevelSelectTextures[0]), true);
        menuSettingsSprite.setTexture(*(selectedMenuIndex == 2 ? menuSettingsTextures[1] : menuSettingsTextures[0]), true);
        menuRankingsSprite.setTexture(*(selectedMenuIndex == 3 ? menuRankingsTextures[1] : menuRankingsTextures[0]), true);
        menuLeaveSprite.setTexture(*(selectedMenuIndex == 4 ? menuLeaveTextures[1] : menuLeaveTextures[0]), true);

        constexpr float SCALE_FACTOR = 1.1f;
        menuPlaySprite.setScale(selectedMenuIndex == 0 ? SCALE_FACTOR : 1.0f, selectedMenuIndex == 0 ? SCALE_FACTOR : 1.0f);
        menuLevelSelectSprite.setScale(selectedMenuIndex == 1 ? SCALE_FACTOR : 1.0f, selectedMenuIndex == 1 ? SCALE_FACTOR : 1.0f);
        menuSettingsSprite.setScale(selectedMenuIndex == 2 ? SCALE_FACTOR : 1.0f, selectedMenuIndex == 2 ? SCALE_FACTOR : 1.0f);
        menuRankingsSprite.setScale(selectedMenuIndex == 3 ? SCALE_FACTOR : 1.0f, selectedMenuIndex == 3 ? SCALE_FACTOR : 1.0f);
        menuLeaveSprite.setScale(selectedMenuIndex == 4 ? SCALE_FACTOR : 1.0f, selectedMenuIndex == 4 ? SCALE_FACTOR : 1.0f);
}
