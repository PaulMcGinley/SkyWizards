//
// Created by Paul McGinley on 31/05/2025.
//

#include "LeaderboardScene.h"

void LeaderboardScene::Update(GameTime gameTime) {
        // std::cout << viewport.getCenter().y << std::endl;
        // if (viewport.getCenter().y < 1500)
        //         viewport.move({0, 50 * gameTime.deltaTime}); // Scroll down the leaderboard

        if (inputManager.IsCancelPressed()) {
                sceneManager.ChangeScene(SceneType::SCENE_MAIN_MENU); // Go back to the main menu
        }
}
void LeaderboardScene::LateUpdate(GameTime gameTime) {}
void LeaderboardScene::Draw(sf::RenderWindow &window, GameTime gameTime) {
        IDraw::Draw(window, "PrgUse", 0, {0,0});
        IDraw::Draw(window, "PrgUse", 49, {0,0});

        sf::View previousView = window.getView(); // Store the previous view
        window.setView(viewport);

        std::vector<Score> scores = gameManager.leaderboard.GetTopNScores(100);

        for (size_t i = 0; i < scores.size(); ++i) {
                const Score& score = scores[i];
                std::string scoreText = std::to_string(i + 1) + ". " + score.name + " - " + std::to_string(score.score) + " (" + score.date + ")";
                IDraw::DrawText(window, "OpenSans-Bold", scoreText, sf::Vector2f(viewport.getSize().x / 2, i * fontSize), Align::CENTER, fontSize, sf::Color::White);
        }

        window.setView(previousView);
}
void LeaderboardScene::InitializeScene() { IScene::InitializeScene(); }
void LeaderboardScene::DestroyScene() {}
void LeaderboardScene::OnScene_Activate() {
        gameManager.leaderboard.Load();
        gameManager.leaderboard.SortScores();
        viewport.setSize(sf::Vector2f(gameManager.getResolutionWidth(), gameManager.getResolutionHeight()));
        viewport.setCenter(gameManager.getResolutionWidth() / 2, gameManager.getResolutionHeight() / 2); // Center the viewport on the screen
}
void LeaderboardScene::OnScene_Deactivate() {}
