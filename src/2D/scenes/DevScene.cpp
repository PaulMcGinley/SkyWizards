//
// Created by Paul McGinley on 10/10/2024.
//

#include "DevScene.h"

// Viewport reference: https://www.sfml-dev.org/tutorials/3.0/graphics/view/#defining-what-the-view-views

void DevScene::update(GameTime gameTime) {

        // Update Scenery
        floatingIsland.update(gameTime);

        // Update entities
        player.update(gameTime);

        // Update the camera
        sf::Vector2f viewCenter = player.position + sf::Vector2f(250,0);
        viewport.setCenter(viewCenter); // Center the viewport on the player
        //viewport.setCenter(player.position);
}

void DevScene::lateUpdate(GameTime gameTime) {
    player.lateUpdate(gameTime);
    floatingIsland.lateUpdate(gameTime);
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

void DevScene::initializeScene() {
        // Initialize the player
        player.position = sf::Vector2f(620, 590);

        // Initialize the view
        viewport.setSize(sf::Vector2f(1920,1080)); // Set the view size to the window size TODO: Change this from hardcoded
        viewport.setCenter(player.position); // Center the viewport on the player

        IScene::initializeScene(); // Call the parent class to set the scene as initialized
}

void DevScene::destroyScene() {
    // When the scene is finished
}

void DevScene::onScene_Active() {
    // When the scene starts
}

void DevScene::onScene_Deactivate() {
    // When the scene ends
}

