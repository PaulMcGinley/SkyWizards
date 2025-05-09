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
                // Get the texture library map entry
                auto lib = asset_manager.TextureLibraries.find(textureLibraryName);
                if (lib == asset_manager.TextureLibraries.end())
                        return; // Texture library not found

                // Get the texture library
                const auto* textureLib = lib->second.get();
                if (index < 0 || index >= textureLib->entryCount)
                        return; // Index out of bounds

                const auto& entry = textureLib->entries[index]; // Get the texture entry
                const auto& texture = entry.texture; // Get the texture
                sf::Vector2f size(static_cast<float>(texture.getSize().x), static_cast<float>(texture.getSize().y));

                // Calculate draw position with offsets
                sf::Vector2f drawPosition = position;
                drawPosition.x += entry.xOffset;
                drawPosition.y += entry.yOffset;
                drawPosition.x = std::round(drawPosition.x);
                drawPosition.y = std::round(drawPosition.y);

                // Get current view bounds
                const sf::View& currentView = window.getView();
                sf::FloatRect viewBounds(
                    currentView.getCenter() - currentView.getSize() / 2.f,
                    currentView.getSize()
                );

                // Object bounds
                sf::FloatRect objectBounds(drawPosition, size);

                // Skip drawing if outside view
                if (!viewBounds.intersects(objectBounds))
                        return;

                // Draw texture
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
