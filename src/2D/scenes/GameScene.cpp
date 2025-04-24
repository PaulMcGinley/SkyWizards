//
// Created by Paul McGinley on 24/04/2025.
//

#include "GameScene.h"

#include "managers/SceneManager.h"

GameScene::GameScene(std::string name)
        : IScene()
        , mapName(std::move(name))
        , map(nullptr) {

}

void GameScene::Update(GameTime gameTime) {

}

void GameScene::LateUpdate(GameTime gameTime) {

}

void GameScene::Draw(sf::RenderWindow &window, GameTime gameTime) {
        if (!IsInitialized()) {
                // Draw loading screen
                return;
        }

        // Draw game
}

void GameScene::InitializeScene() {
        LoadAssets();

        // TODO: Don't initialize the scene if assets are loading
        IScene::InitializeScene();
}

void GameScene::DestroyScene() {

}

void GameScene::OnScene_Active() {

}

void GameScene::OnScene_Deactivate() {

}

void GameScene::LoadAssets() {
        // Check if the map exists in the asset manager
        if (asset_manager.Maps.contains(mapName)) {
                // Get the map from the asset manager
                map = &asset_manager.Maps[mapName];
        } else {
                std::cerr << "Map " << mapName << " does not exist in the asset manager." << std::endl;
                SceneManager::GetInstance().ChangeScene(SceneType::SCENE_MAIN_MENU);
                return;
        }
}
