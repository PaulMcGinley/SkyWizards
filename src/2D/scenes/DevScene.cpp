//
// Created by Paul McGinley on 10/10/2024.
//

#include "DevScene.h"

void DevScene::Update(GameTime gameTime) {

    player.Update(gameTime);

    floatingIsland.Update(gameTime);
    // Update the scene
}

void DevScene::LateUpdate(GameTime gameTime) {

    player.LateUpdate(gameTime);
    floatingIsland.LateUpdate(gameTime);
    // Late update the scene
}

void DevScene::Draw(sf::RenderWindow& window, GameTime gameTime) {
    floatingIsland.Draw(window, gameTime);
    player.Draw(window, gameTime);
    // Draw the scene
}

void DevScene::Scene_Init() {

    player.position = sf::Vector2f(620, 590);
}

void DevScene::Scene_Destroy() {
    // When the scene is finished
}

void DevScene::OnScene_Active() {
    // When the scene starts
}

void DevScene::OnScene_Deactive() {
    // When the scene ends
}

