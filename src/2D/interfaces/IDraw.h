//
// Created by Paul McGinley on 30/09/2024.
//

#ifndef IDRAW_H
#define IDRAW_H

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include "managers/AssetManager.h"
#include "models/GameTime.h"

class IDraw {
public:
        virtual ~IDraw() = default;

        // Unhandled draw function: This function should be implemented by the derived class
        virtual void Draw(sf::RenderWindow& window, GameTime gameTime) = 0;

        // Handled draw function: Pass in a texture library name, index, and position to draw the texture
        void Draw(sf::RenderWindow& window, const std::string& textureLibraryName, const int index, const sf::Vector2f position) const {
                auto sizeU = asset_manager.TextureLibraries[textureLibraryName]->entries[index].texture.getSize();
                sf::Vector2f size(static_cast<float>(sizeU.x), static_cast<float>(sizeU.y));
                sf::RectangleShape rect(size);
                rect.setTexture(&asset_manager.TextureLibraries[textureLibraryName]->entries[index].texture);
                rect.setPosition(position);
                window.draw(rect);
        }

        AssetManager& asset_manager = AssetManager::GetInstance();
};

#endif //IDRAW_H
