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
        void UpdateMenuSelection();
        sf::VertexArray backgroundQuad{sf::Quads, 4};

        sf::Texture* title1Texture;
        sf::Texture* title2Texture;
        sf::Texture* title3Texture;

        sf::Sprite title1Sprite;
        sf::Sprite title2Sprite;
        sf::Sprite title3Sprite;

        int selectedMenuItem = 0;

        sf::Texture* menuPlayTextures[2];
        sf::Texture* menuLevelSelectTextures[2];
        sf::Texture* menuSettingsTextures[2];
        sf::Texture* menuRankingsTextures[2];
        sf::Texture* menuLeaveTextures[2];

        sf::Sprite menuPlaySprite;
        sf::Sprite menuLevelSelectSprite;
        sf::Sprite menuSettingsSprite;
        sf::Sprite menuRankingsSprite;
        sf::Sprite menuLeaveSprite;
};



#endif //MAINMENU_H
