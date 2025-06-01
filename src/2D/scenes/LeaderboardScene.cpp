//
// Created by Paul McGinley on 31/05/2025.
//

#include "LeaderboardScene.h"

void LeaderboardScene::Update(GameTime gameTime) {
        if (inputManager.IsKeyDown(sf::Keyboard::Down) || inputManager.IsKeyDown(sf::Keyboard::S)) {
                viewport.move({0, 400 * gameTime.deltaTime}); // Scroll down the leaderboard
        } else if (inputManager.IsKeyDown(sf::Keyboard::Up) || inputManager.IsKeyDown(sf::Keyboard::W)) {
                viewport.move({0, -400 * gameTime.deltaTime}); // Scroll up the leaderboard
        }

        if (inputManager.IsCancelPressed()) {
                sceneManager.ChangeScene(SceneType::SCENE_MAIN_MENU); // Go back to the main menu
        }
}
void LeaderboardScene::LateUpdate(GameTime gameTime) {}
void LeaderboardScene::Draw(sf::RenderWindow &window, GameTime gameTime) {
        // Graphics
        IDraw::Draw(window, "PrgUse", 0, {0,0});
        IDraw::Draw(window, "PrgUse", 49, {0,0});
        IDraw::Draw(window, "PrgUse", 50, {gameManager.getResolutionWidth()/2 - static_cast<float>(assetManager.TextureLibraries["PrgUse"].get()->entries[50].texture.getSize().x / 2),0});

        // Titles
        IDraw::DrawText(window, "OpenSans-Bold","RANK", sf::Vector2f(rankTitleX, titleY), Align::CENTER, 35, defaultColour);
        IDraw::DrawText(window, "OpenSans-Bold","NAME", sf::Vector2f(nameTitleX, titleY), Align::CENTER, 35, defaultColour);
        IDraw::DrawText(window,  "OpenSans-Bold", "SCORE", sf::Vector2f(scoreTitleX, titleY), Align::CENTER, 35, defaultColour);
        IDraw::DrawText(window, "OpenSans-Bold","DATE", sf::Vector2f(dateTitleX, titleY), Align::CENTER, 35, defaultColour);

        sf::View previousView = window.getView(); // Store the previous view
        window.setView(viewport);

        // Leaderboard entries
        for (size_t i = 0; i < scores.size(); ++i) {
                sf::Color textColour = defaultColour;
                float entryY = i * fontSize - viewport.getCenter().y + gameManager.getResolutionHeight() / 2;
                if (i == 0) {
                        textColour = firstPlaceColour;
                } else if (i == 1) {
                        textColour = secondPlaceColour;
                } else if (i == 2) {
                        textColour = thirdPlaceColour;
                }

                float alpha = 255.0f;
                if (entryY < fadeOutStart) {
                        alpha = (entryY <= fadeOutEnd) ? 0 : 255.0f * ((entryY - fadeOutEnd) / (fadeOutStart - fadeOutEnd));
                }
                textColour.a = static_cast<sf::Uint8>(alpha);

                const Score& score = scores[i];
                IDraw::DrawText(window, "OpenSans-Bold", std::to_string(i+1), sf::Vector2f(rankTitleX, i * fontSize), Align::CENTER, fontSize, textColour, false);
                IDraw::DrawText(window, "OpenSans-Bold", score.name, sf::Vector2f(nameTitleX, i * fontSize), Align::CENTER, fontSize, textColour, false);
                IDraw::DrawText(window, "OpenSans-Bold", std::to_string(score.score), sf::Vector2f(scoreTitleX, i * fontSize), Align::CENTER, fontSize, textColour, false);
                IDraw::DrawText(window, "OpenSans-Bold", score.date, sf::Vector2f(dateTitleX, i * fontSize), Align::CENTER, fontSize, textColour, false);
        }

        window.setView(previousView);
}
void LeaderboardScene::InitializeScene() { IScene::InitializeScene(); }
void LeaderboardScene::DestroyScene() {}
void LeaderboardScene::OnScene_Activate() {
        gameManager.leaderboard.Load();
        gameManager.leaderboard.SortScores();
        scores = gameManager.leaderboard.GetTopNScores(100);

        viewport.setSize(sf::Vector2f(gameManager.getResolutionWidth(), gameManager.getResolutionHeight()));
        viewport.setCenter(gameManager.getResolutionWidth() / 2, gameManager.getResolutionHeight() / 2 - 100); // Center the viewport on the screen
}
void LeaderboardScene::OnScene_Deactivate() {}
