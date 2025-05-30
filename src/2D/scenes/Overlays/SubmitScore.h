//
// Created by Paul McGinley on 30/05/2025.
//

#ifndef SUBMITSCORE_H
#define SUBMITSCORE_H

#include "interfaces/IScene.h"
#include "objects/Player.h"

class SubmitScore : public IScene {
public:
        void Update(GameTime gameTime) override;
        void LateUpdate(GameTime gameTime) override;
        void Draw(sf::RenderWindow& window, GameTime gameTime) override;
        void InitializeScene() override;
        void DestroyScene() override;
        void OnScene_Activate() override;
        void OnScene_Deactivate() override;

        void SetPlayer(Player* player);

private:
        char name[5] = {0}; // Player's name, null-terminated
        Player* player;

        int index; // Index of the character being edited

        std::string GetDateTimeString();

};

#endif //SUBMITSCORE_H
