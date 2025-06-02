//
// Created by Paul McGinley on 01/06/2025.
//

#include "GiveUpOverlay.h"
void GiveUpOverlay::Update(GameTime gameTime) {
        if (!active)
                return;

        if (inputManager.IsCancelPressed())
                active = false;

        if (inputManager.IsConfirmPressed()) {
                active = false;
                sceneManager.ChangeScene(SceneType::SCENE_MAIN_MENU);
        }
}
void GiveUpOverlay::LateUpdate(GameTime gameTime) {}
void GiveUpOverlay::Draw(sf::RenderWindow &window, GameTime gameTime) {
        if (!active)
                return;

        IDraw::Draw(window, "PrgUse", 51, sf::Vector2f(0, 0));
        //window.draw(shade); // Draw the shade to bring focus to the overlay

        IDraw::DrawText(window, "OpenSans-ExtraBold", "Are you sure you want to give up?", sf::Vector2f(gameManager.getResolutionWidth()/2, 150), Align::CENTER, 50, sf::Color::White, 20);
        IDraw::DrawText(window, "OpenSans-Bold", "Press CONFIRM to return to the main menu.", sf::Vector2f(gameManager.getResolutionWidth()/2, gameManager.getResolutionHeight()/2 - 50), Align::CENTER, 50, sf::Color::Red, 20);
        IDraw::DrawText(window, "OpenSans-Bold", "Press CANCEL to return to the the game.", sf::Vector2f(gameManager.getResolutionWidth()/2, gameManager.getResolutionHeight()/2 + 50), Align::CENTER, 50, sf::Color::Green, 20);
}
void GiveUpOverlay::InitializeScene() {
        index = 0;
        active = false;

        sf::Vector2i screenSize ={static_cast<int>(gameManager.getResolutionWidth()), static_cast<int>(gameManager.getResolutionHeight())};
        shade.setSize(sf::Vector2f(screenSize.x, screenSize.y));
        shade.setFillColor(sf::Color(50, 0, 0, 150)); // Semi-transparent black shade

        IScene::InitializeScene();
}
void GiveUpOverlay::DestroyScene() {}
void GiveUpOverlay::OnScene_Activate() {
        index = 0;
}
void GiveUpOverlay::OnScene_Deactivate() {
        index = 0;
}
bool GiveUpOverlay::SetActive() { active = true; }
bool GiveUpOverlay::IsActive() { return active; }
