//
// Created by Paul McGinley on 12/10/2024.
//

#ifndef TIMAGE_H
#define TIMAGE_H
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/VertexArray.hpp>

class TImage {
public:
  sf::Texture image;
  sf::Sprite sprite;

  int xOffset = 0;
  int yOffset = 0;
  int width = 0;
  int height = 0;

  sf::VertexArray quads{sf::Quads, 4};

  void CreateTexture() {

  }

};



#endif //TIMAGE_H
