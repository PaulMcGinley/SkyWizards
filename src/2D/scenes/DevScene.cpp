//
// Created by Paul McGinley on 10/10/2024.
//

#include "DevScene.h"

// Viewport reference: https://www.sfml-dev.org/tutorials/3.0/graphics/view/#defining-what-the-view-views

void DevScene::Update(GameTime gameTime) {

        // Update Scenery
        floatingIsland.Update(gameTime);

        // Update entities
        player.Update(gameTime);

        // Update the camera
        sf::Vector2f viewCenter = player.position + sf::Vector2f(250,0);
        viewport.setCenter(viewCenter); // Center the viewport on the player
        //viewport.setCenter(player.position);
}

void DevScene::LateUpdate(GameTime gameTime) {
    player.LateUpdate(gameTime);
    floatingIsland.LateUpdate(gameTime);
}

void DevScene::Draw(sf::RenderWindow& window, GameTime gameTime) {
        // Set the view
        window.setView(viewport);

        // Draw the scenery
        floatingIsland.Draw(window, gameTime);
        player.Draw(window, gameTime);

        // Draw the UI
        window.setView(window.getDefaultView());
        player.health.Draw(window, gameTime);
}

void DevScene::Scene_Init(sf::RenderWindow& window) {
        // Initialize the player
        player.position = sf::Vector2f(620, 590);

        // Initialize the view
        viewport.setSize(sf::Vector2f(1920,1080)); // Set the view size to the window size TODO: Change this from hardcoded
        viewport.setCenter(player.position); // Center the viewport on the player
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

