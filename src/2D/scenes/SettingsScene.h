//
// Created by Paul McGinley on 16/05/2025.
//

#ifndef SETTINGSSCENE_H
#define SETTINGSSCENE_H

#include <SFML/Graphics/Sprite.hpp>


#include "interfaces/IScene.h"

class SettingsScene : public IScene{
public:
        SettingsScene();
        ~SettingsScene() override = default;
        void Update(GameTime gameTime) override;
        void LateUpdate(GameTime gameTime) override;
        void Draw(sf::RenderWindow &window, GameTime gameTime) override;
        void OnScene_Active() override;
        void OnScene_Deactivate() override;
        void DestroyScene() override;
        void InitializeScene() override;

private:
        bool tempIsFullscreen = false;
        bool tempIsVSync = false;
        float tempMusicPercent = 0.0f;
        float tempSfxPercent = 0.0f;

        int selectedOption = 0;

        sf::Texture* backgroundTexture;
        sf::Texture* regionTexture;

        sf::Texture* numbersTexture[10];
        sf::Texture* percentTexture;
        sf::Texture* checkBoxTexture[2];
        sf::Texture* modeTexture;
        sf::Texture* modeOptionTexture[2];
        sf::Texture* vsyncTexture;
        sf::Texture* vsyncOptionTexture[2];
        sf::Texture* musicTexture;
        sf::Texture* sfxTexture;
        sf::Texture* arrowLeftTexture[2];
        sf::Texture* arrowRightTexture[2];

        sf::Sprite backgroundSprite;
        sf::Sprite regionSprite;

        sf::Sprite modeSprite;
        sf::Sprite modeOptionSprite[2];
        sf::Sprite modeOptionArrowLeftSprite[2];
        sf::Sprite modeOptionArrowRightSprite[2];
        sf::Sprite vsyncSprite;
        sf::Sprite vsyncOptionSprite[2];
        sf::Sprite vsyncOptionArrowLeftSprite[2];
        sf::Sprite vsyncOptionArrowRightSprite[2];
        sf::Sprite musicSprite;
        sf::Sprite sfxSprite;
};



#endif //SETTINGSSCENE_H
