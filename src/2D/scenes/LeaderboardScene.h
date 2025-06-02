//
// Created by Paul McGinley on 31/05/2025.
//

#ifndef LEADERBOARDSCENE_H
#define LEADERBOARDSCENE_H

#include "interfaces/IScene.h"

class LeaderboardScene : public IScene {
public:
        void Update(GameTime gameTime) override;
        void LateUpdate(GameTime gameTime) override;
        void Draw(sf::RenderWindow& window, GameTime gameTime) override;
        void InitializeScene() override;
        void DestroyScene() override;
        void OnScene_Activate() override;
        void OnScene_Deactivate() override;
        void ScrollToTop();
        void ScrollToBottom();
        void ScrollToLatest();

private:
        std::vector<Score> scores;
        int fontSize = 40;
        sf::View viewport;

        sf::Color firstPlaceColour = sf::Color(255, 215, 0);     // Gold
        sf::Color secondPlaceColour = sf::Color(192, 192, 192);  // Silver
        sf::Color thirdPlaceColour = sf::Color(205, 127, 50);    // Bronze
        sf::Color defaultColour = sf::Color::White;

        float titleY = 15;
        float rankTitleX = 300;
        float nameTitleX = 600;
        float scoreTitleX = 1000;
        float dateTitleX = 1500;

        float fadeOutStart = 100.f;
        float fadeOutEnd = 70.f;

        bool highlightLatest = false;
        int latestIndex = -1;
};

#endif //LEADERBOARDSCENE_H
