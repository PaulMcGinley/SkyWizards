//
// Created by Paul McGinley on 10/10/2024.
//

#ifndef DEVSCENE_H
#define DEVSCENE_H

#include "interfaces/IScene.h"
#include "objects/Player.h"

#include <SFML/Graphics/Shader.hpp>

class DevScene : public IScene {

public:
        void Update(GameTime gameTime) override;
        void LateUpdate(GameTime gameTime) override;
        void Draw(sf::RenderWindow& window, GameTime gameTime) override;
        void InitializeScene() override;
        void DestroyScene() override;
        void OnScene_Active() override;
        void OnScene_Deactivate() override;

private:
        Player player = Player();
        sf::View viewport;
};

#endif //DEVSCENE_H
