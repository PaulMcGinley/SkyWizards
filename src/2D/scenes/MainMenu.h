//
// Created by Paul McGinley on 25/01/2025.
//

#ifndef MAINMENU_H
#define MAINMENU_H


#include <SFML/Graphics/Sprite.hpp>


#include "interfaces/IScene.h"


class MainMenu : public IScene {
public:
        MainMenu();

        void Update(GameTime gameTime) override;
        void LateUpdate(GameTime gameTime) override;
        void Draw(sf::RenderWindow& window, GameTime gameTime) override;
        void InitializeScene() override;
        void DestroyScene() override;
        void OnScene_Active() override;
        void OnScene_Deactivate() override;

private:
        sf::VertexArray backgroundQuad{sf::Quads, 4};

        sf::Texture* title1Texture;
        sf::Texture* title2Texture;
        sf::Texture* title3Texture;

        sf::Sprite title1Sprite;
        sf::Sprite title2Sprite;
        sf::Sprite title3Sprite;
};



#endif //MAINMENU_H
