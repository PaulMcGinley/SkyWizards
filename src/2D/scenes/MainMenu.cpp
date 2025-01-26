//
// Created by Paul McGinley on 25/01/2025.
//

#include "MainMenu.h"

void MainMenu::Update(GameTime gameTime) {

}
void MainMenu::LateUpdate(GameTime gameTime) {

}
void MainMenu::Draw(sf::RenderWindow &window, GameTime gameTime) {

        window.draw(backgroundQuad, &AssetManager::getInstance().ProgramUsage.entries[0].texture);
}
void MainMenu::Scene_Init(sf::RenderWindow& window) {

        // Set up the background quad to cover the entire window
        backgroundQuad[0].position = sf::Vector2f(0, 0);
        backgroundQuad[1].position = sf::Vector2f(1920, 0);
        backgroundQuad[2].position = sf::Vector2f(1920, 1080);
        backgroundQuad[3].position = sf::Vector2f(0, 1080);

        backgroundQuad[0].texCoords = sf::Vector2f(0, 0);
        backgroundQuad[1].texCoords = sf::Vector2f(1920, 0);
        backgroundQuad[2].texCoords = sf::Vector2f(1920, 1080);
        backgroundQuad[3].texCoords = sf::Vector2f(0, 1080);
}
void MainMenu::Scene_Destroy() {

}
void MainMenu::OnScene_Active() {

}
void MainMenu::OnScene_Deactive() {

}
