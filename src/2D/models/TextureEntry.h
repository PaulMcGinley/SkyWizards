//
// Created by Paul McGinley on 30/09/2024.
//

#ifndef IMAGEENTRY_H
#define IMAGEENTRY_H

#include <vector>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <cstdint>

struct TextureEntry {
        int xOffset = 0;
        int yOffset = 0;
        std::vector<uint8_t> data; // PNG data as byte array // TODO: Load this as dynamic memory so we can delete the pointer and not have an empty variable hanging around
        sf::Texture texture;
        sf::VertexArray texQuad{sf::Quads, 4};

        // Calculate the texture quads when using TextureEntry as a single image (mainly testing atm)
        void CalculateQuads() {
                texQuad[0].texCoords = {0, 0};
                texQuad[1].texCoords = {static_cast<float>(texture.getSize().x), 0};
                texQuad[2].texCoords = {static_cast<float>(texture.getSize().x), static_cast<float>(texture.getSize().y)};
                texQuad[3].texCoords = {0, static_cast<float>(texture.getSize().y)};

                texQuad[0].position = {static_cast<float>(xOffset), static_cast<float>(yOffset)};
                texQuad[1].position = {static_cast<float>(xOffset + texture.getSize().x), static_cast<float>(yOffset)};
                texQuad[2].position = {static_cast<float>(xOffset + texture.getSize().x), static_cast<float>(yOffset + texture.getSize().y)};
                texQuad[3].position = {static_cast<float>(xOffset), static_cast<float>(yOffset + texture.getSize().y)};
        }
};

#endif //IMAGEENTRY_H
