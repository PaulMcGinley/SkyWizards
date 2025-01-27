//
// Created by Paul McGinley on 10/10/2024.
//

#ifndef DEVSCENE_H
#define DEVSCENE_H

#include "interfaces/IScene.h"
#include "models/BackgroundSceneryObject.h"
#include "objects/Player.h"

#include <SFML/Graphics/Shader.hpp>

class DevScene : public IScene {

public:
        void update(GameTime gameTime) override;
        void lateUpdate(GameTime gameTime) override;
        void draw(sf::RenderWindow& window, GameTime gameTime) override;
        void initializeScene() override;
        void destroyScene() override;
        void onScene_Active() override;
        void onScene_Deactivate() override;

private:
        TextureLibrary& floatingIslandLibrary = AssetManager::getInstance().backgroundIslands;
        BackgroundSceneryObject floatingIsland = BackgroundSceneryObject(floatingIslandLibrary, 0, 36, 50, sf::Vector2f(0, -500));
        Player player = Player();
        sf::View viewport;
};

#endif //DEVSCENE_H
