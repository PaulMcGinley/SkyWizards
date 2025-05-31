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

private:
        bool autoScroll = true;
        float currentYOffset = 0.f;

        int fontSize = 70;

        sf::View viewport;
};

#endif //LEADERBOARDSCENE_H
