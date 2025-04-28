//
// Created by Paul McGinley on 08/10/2024.
//

#pragma once

#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include "interfaces/IScene.h"

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>

class SplashScreen : public IScene {
public:
        SplashScreen();
        ~SplashScreen() override = default;

        void Update(GameTime gameTime) override;
        void LateUpdate(GameTime gameTime) override;
        void Draw(sf::RenderWindow& window, GameTime gameTime) override;
        void InitializeScene() override;
        void DestroyScene() override;
        void OnScene_Active() override;
        void OnScene_Deactivate() override;

private:
        // Progress bar variables
        int TargetValue = 12;
        int CurrentValue = 0;

        // Text variables
        sf::Font font;
        sf::Text text;
        int textYPosition = 900;

        sf::Font copyrightFont;
        sf::Text copyRightText;

        // Textures
        sf::Texture background;
        sf::Texture frame;
        sf::Texture progress;

        sf::VertexArray backgroundQuad = sf::VertexArray(sf::Quads, 4);
        sf::VertexArray frameQuad = sf::VertexArray(sf::Quads, 4);
        sf::VertexArray progressQuad = sf::VertexArray(sf::Quads, 4);

        float loadCompletionTime = -1;

        void InitializeLibraries();
        void loadLevelObjects(const std::string& directoryPath);
        void loadMaps(const std::string & directoryPath);
        void loadFonts(const std::string & directoryPath);
};

#endif //SPLASHSCREEN_H
