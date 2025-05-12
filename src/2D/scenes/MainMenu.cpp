//
// Created by Paul McGinley on 25/01/2025.
//

#include "MainMenu.h"

#include "GameScene.h"
#include "managers/InputManager.cpp"
#include "managers/SceneManager.h"

MainMenu::MainMenu() {}
void MainMenu::Update(GameTime gameTime) {
        if (InputManager::getInstance().isKeyDown(sf::Keyboard::Key::Space)) {
                auto scenePtr = scene_manager.GetScene(SceneType::SCENE_GAME);
                auto gameScene = std::dynamic_pointer_cast<GameScene>(scenePtr);
                if (gameScene) {
                        gameScene->LoadMap("Mob_Test");
                }
                scene_manager.ChangeScene(SceneType::SCENE_GAME);
        }

        float t = gameTime.total_game_time;     // time in seconds
        float amplitude = 0.2f;                 // scale amplitude
        float speed = 2.0f;                     // speed of animation
        float offset = 0.7f;                    // offset for the second sprite (so they don't scale the same)

        float scale1 = 1.0f + amplitude * std::sin(t * speed + (offset*2));
        float scale2 = 1.0f + amplitude * std::sin(t * speed + offset);
        float scale3 = 1.0f + amplitude * std::sin(t * speed);

        title1Sprite.setScale(scale1, scale1);
        title2Sprite.setScale(scale2, scale2);
        title3Sprite.setScale(scale3, scale3);
}
void MainMenu::LateUpdate(GameTime gameTime) {}
void MainMenu::Draw(sf::RenderWindow &window, GameTime gameTime) {
        window.draw(backgroundQuad, &asset_manager.TextureLibraries["PrgUse"].get()->entries[0].texture);

        window.draw(title1Sprite);
        window.draw(title2Sprite);
        window.draw(title3Sprite);
}
void MainMenu::InitializeScene() {
        float screenWidth = game_manager.getResolution().x;
        float screenHeight = game_manager.getResolution().y;

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
void MainMenu::OnScene_Active() {
        float screenWidth = game_manager.getResolution().x;
        float screenHeight = game_manager.getResolution().y;

        // TODO: Move this to the TextureLibraries map
        //title1Texture = &asset_manager.TextureLibraries["PrgUse"]->entries[7].texture;
        title1Texture = &asset_manager.TextureLibraries["PrgUse"].get()->entries[7].texture;
        title2Texture = &asset_manager.TextureLibraries["PrgUse"].get()->entries[6].texture;
        title3Texture = &asset_manager.TextureLibraries["PrgUse"].get()->entries[5].texture;

        title1Sprite.setTexture(*title1Texture, true);
        title2Sprite.setTexture(*title2Texture, true);
        title3Sprite.setTexture(*title3Texture, true);

        title1Sprite.setOrigin(title1Texture->getSize().x / 2, title1Texture->getSize().y / 2);
        title2Sprite.setOrigin(title2Texture->getSize().x / 2, title2Texture->getSize().y / 2);
        title3Sprite.setOrigin(title3Texture->getSize().x / 2, title3Texture->getSize().y / 2);

        title1Sprite.setPosition(screenWidth/2, (screenHeight/2) - 150);
        title2Sprite.setPosition(screenWidth/2, (screenHeight/2));
        title3Sprite.setPosition(screenWidth/2, (screenHeight/2) + 130);

        // Set smooth textures to prevent pixelation (Bilinear filtering)
        title1Texture->setSmooth(true);
        title2Texture->setSmooth(true);
        title3Texture->setSmooth(true);
}
void MainMenu::OnScene_Deactivate() {}
