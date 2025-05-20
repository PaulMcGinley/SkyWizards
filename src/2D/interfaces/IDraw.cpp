//
// Created by Paul McGinley on 15/05/2025.
//

#include "IDraw.h"

void IDraw::Draw(sf::RenderWindow &window, const std::string &textureLibraryName, const int index,  const sf::Vector2f position) const {
        // Get the Texture Library
        const auto library = assetManager.TextureLibraries.find(textureLibraryName);
        // Check if the library exists
        if (library == assetManager.TextureLibraries.end()) {
                std::cerr << "Tried to draw a texture from a non-existent library: " << textureLibraryName << std::endl;
                return;
        }

        // Get the Texture Library content
        const auto *libraryContent = library->second.get();
        // Check if the library content is valid
        if (index < 0 || index >= libraryContent->entryCount) {
                std::cerr << "Tried to draw a texture from library: " << textureLibraryName << " with an index out of bounds: " << index << std::endl;
                return;
        }

        // Get the Texture Library entry
        const auto &entry = libraryContent->entries[index];
        // Get the texture
        const auto &texture = entry.texture;
        // Get the size of the texture
        const sf::Vector2f size(static_cast<float>(texture.getSize().x), static_cast<float>(texture.getSize().y));

        // Calculate the texture draw position with offsets
        sf::Vector2f drawPosition = position;
        drawPosition.x += entry.xOffset;
        drawPosition.y += entry.yOffset;
        // Rounding helps with pixel-perfect rendering
        drawPosition.x = std::round(drawPosition.x);
        drawPosition.y = std::round(drawPosition.y);

        // Get the current Viewport
        const sf::View &view = window.getView();
        // Create a bounding box for the view (will be used to check if the object is in view)
        const sf::FloatRect viewBounds(view.getCenter() - view.getSize() / 2.f, view.getSize());

        // Create a bounding box for the object to be drawn
        sf::FloatRect objectBounds(drawPosition, size);

        // Check if the object is not in view, if so, don't draw it
        if (!viewBounds.intersects(objectBounds))
                return;

        // Draw texture
        sf::RectangleShape textureRectangle(size);      // Create a rectangle shape with the size of the texture
        textureRectangle.setTexture(&texture);          // Set the texture of the rectangle shape
        textureRectangle.setPosition(drawPosition);     // Set the position of the rectangle shape
        window.draw(textureRectangle);                  // Draw the rectangle shape to the window
}
void IDraw::DrawBlend(sf::RenderWindow &window, const std::string &textureLibraryName, const int index, const sf::Vector2f position, const sf::BlendMode &blend) const {
        // Get the Texture Library
        const auto library = assetManager.TextureLibraries.find(textureLibraryName);
        if (library == assetManager.TextureLibraries.end()) {
                std::cerr << "Tried to draw a texture from a non-existent library: " << textureLibraryName << " (blending)" << std::endl;
                return;
        }

        const auto *libraryContent = library->second.get();
        if (index < 0 || index >= libraryContent->entryCount) {
                std::cerr << "Tried to draw a texture from library: " << textureLibraryName << " with an index out of bounds: " << index  << " (blending)" << std::endl;
                return;
        }

        // Get the Texture Library entry
        const auto &entry = libraryContent->entries[index];
        // Get the texture
        const auto &texture = entry.texture;
        // Get the size of the texture
        const sf::Vector2f size(static_cast<float>(texture.getSize().x), static_cast<float>(texture.getSize().y));

        // Calculate the texture draw position with offsets
        sf::Vector2f drawPosition = position;
        drawPosition.x += entry.xOffset;
        drawPosition.y += entry.yOffset;
        // Rounding helps with pixel-perfect rendering
        drawPosition.x = std::round(drawPosition.x);
        drawPosition.y = std::round(drawPosition.y);

        // Get the current Viewport
        const sf::View &view = window.getView();
        // Create a bounding box for the view (will be used to check if the object is in view)
        const sf::FloatRect viewBounds(view.getCenter() - view.getSize() / 2.f, view.getSize());

        // Create a bounding box for the object to be drawn
        sf::FloatRect objectBounds(drawPosition, size);

        // Check if the object is not in view, if so, don't draw it
        if (!viewBounds.intersects(objectBounds))
                return;

        // Draw texture with blending
        sf::RectangleShape textureRectangle(size);      // Create a rectangle shape with the size of the texture
        textureRectangle.setTexture(&texture);          // Set the texture of the rectangle shape
        textureRectangle.setPosition(drawPosition);     // Set the position of the rectangle shape
        window.draw(textureRectangle, blend);         // Draw the rectangle shape to the window with blending
}
