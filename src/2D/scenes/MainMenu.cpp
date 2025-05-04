//
// Created by Paul McGinley on 25/01/2025.
//

#include "MainMenu.h"

#include "GameScene.h"
#include "managers/InputManager.cpp"
#include "managers/SceneManager.h"

void MainMenu::Update(GameTime gameTime) {
        if (InputManager::getInstance().isKeyDown(sf::Keyboard::Key::Space)) {
                auto scenePtr = SceneManager::GetInstance().GetScene(SceneType::SCENE_GAME);
                auto gameScene = std::dynamic_pointer_cast<GameScene>(scenePtr);
                if (gameScene) {
                        gameScene->LoadMap("test3");
                }
                SceneManager::GetInstance().ChangeScene(SceneType::SCENE_GAME);
        }

}
void MainMenu::LateUpdate(GameTime gameTime) {

}
void MainMenu::Draw(sf::RenderWindow &window, GameTime gameTime) {

        window.draw(backgroundQuad, &AssetManager::GetInstance().programUsage.entries[0].texture);
}
void MainMenu::InitializeScene() {

        // Set up the background quad to cover the entire window
        backgroundQuad[0].position = sf::Vector2f(0, 0);
        backgroundQuad[1].position = sf::Vector2f(1920, 0);
        backgroundQuad[2].position = sf::Vector2f(1920, 1080);
        backgroundQuad[3].position = sf::Vector2f(0, 1080);

        backgroundQuad[0].texCoords = sf::Vector2f(0, 0);
        backgroundQuad[1].texCoords = sf::Vector2f(1920, 0);
        backgroundQuad[2].texCoords = sf::Vector2f(1920, 1080);
        backgroundQuad[3].texCoords = sf::Vector2f(0, 1080);

        IScene::InitializeScene(); // Call the parent class to set the scene as initialized
}
void MainMenu::DestroyScene() {

}
void MainMenu::OnScene_Active() {

}
void MainMenu::OnScene_Deactivate() {

}
