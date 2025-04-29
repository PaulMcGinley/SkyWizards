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
                // Skip drawing if the texture library or index doesn't exist
                if (!asset_manager.TextureLibraries.contains(textureLibraryName) || index >= asset_manager.TextureLibraries[textureLibraryName]->entryCount)
                        return;

                // Get texture and size
                auto const & texture = asset_manager.TextureLibraries[textureLibraryName]->entries[index].texture;
                auto sizeU = texture.getSize();
                sf::Vector2f size(static_cast<float>(sizeU.x), static_cast<float>(sizeU.y));

                // Calculate the actual draw position with offsets
                sf::Vector2f drawPosition = position;
                drawPosition.x += asset_manager.TextureLibraries[textureLibraryName]->entries[index].xOffset;
                drawPosition.y += asset_manager.TextureLibraries[textureLibraryName]->entries[index].yOffset;

                // Get the current view bounds
                sf::View currentView = window.getView();
                sf::FloatRect viewBounds(
                    currentView.getCenter() - currentView.getSize() / 2.f,
                    currentView.getSize()
                );

                // Create the bounds for the object
                sf::FloatRect objectBounds(drawPosition, size);

                // If the object is outside the view bounds, skip drawing
                if (!viewBounds.intersects(objectBounds))
                        return;

                // Draw the texture
                sf::RectangleShape rect(size);
                rect.setTexture(&texture);
                rect.setPosition(drawPosition);
                window.draw(rect);
        }

        void DrawBlend(sf::RenderWindow& window, const std::string& textureLibraryName, const int index, const sf::Vector2f position, const sf::BlendMode &blend) const {
                auto sizeU = asset_manager.TextureLibraries[textureLibraryName]->entries[index].texture.getSize();
                sf::Vector2f size(static_cast<float>(sizeU.x), static_cast<float>(sizeU.y));
                sf::RectangleShape rect(size);
                rect.setTexture(&asset_manager.TextureLibraries[textureLibraryName]->entries[index].texture);
                sf::Vector2f drawPosition = position;
                drawPosition.x += asset_manager.TextureLibraries[textureLibraryName]->entries[index].xOffset;
                drawPosition.y += asset_manager.TextureLibraries[textureLibraryName]->entries[index].yOffset;
                rect.setPosition(drawPosition);
                window.draw(rect, blend);
        }

        AssetManager& asset_manager = AssetManager::GetInstance();
};

#endif //IDRAW_H
