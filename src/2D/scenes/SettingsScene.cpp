//
// Created by Paul McGinley on 16/05/2025.
//

#include "SettingsScene.h"
#include "managers/InputManager.cpp"

SettingsScene::SettingsScene() {}
void SettingsScene::Update(GameTime gameTime) {
        if (InputManager::getInstance().isKeyPressed(sf::Keyboard::Num7 )|| InputManager::getInstance().isKeyPressed(sf::Keyboard::I)) {
                scene_manager.ChangeScene(SceneType::SCENE_MAIN_MENU);
        }
}
void SettingsScene::LateUpdate(GameTime gameTime) {}
void SettingsScene::Draw(sf::RenderWindow &window, GameTime gameTime) {}
void SettingsScene::OnScene_Active() {}
void SettingsScene::OnScene_Deactivate() {}
void SettingsScene::DestroyScene() {}
void SettingsScene::InitializeScene() { IScene::InitializeScene(); }
