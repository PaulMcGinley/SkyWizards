//
// Created by Paul McGinley on 30/09/2024.
//

#ifndef IDRAW_H
#define IDRAW_H

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <iostream>
#include "managers/AssetManager.h"
#include "models/GameTime.h"
#include "Enumerators/Align.h"

class IDraw {
public:
        virtual ~IDraw() = default;

        // Unhandled draw function: This function should be implemented by the derived class
        virtual void Draw(sf::RenderWindow& window, GameTime gameTime) = 0;

        // Handled draw function: Pass in a texture library name, index, and position to draw the texture
        void Draw(sf::RenderWindow& window, const std::string& textureLibraryName, const int index, const sf::Vector2f position) const;

        // Handled draw function with blend mode: Pass in a texture library name, index, position, and blend mode to draw the texture
        void DrawBlend(sf::RenderWindow& window, const std::string& textureLibraryName, const int index, const sf::Vector2f position, const sf::BlendMode &blend) const;

        void DrawText(sf::RenderWindow& window, const std::string& fontName, const std::string& text, sf::Vector2f position, Align align, unsigned int size = 30, sf::Color color = sf::Color::White, bool outline = true, float outlineThickness = 1.f) const;

        // Get the asset manager instance
        // Anything that inherits from IDraw can access the asset manager
        AssetManager& assetManager = AssetManager::GetInstance();
};

#endif //IDRAW_H
