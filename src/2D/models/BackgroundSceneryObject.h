//
// Created by Paul McGinley on 10/10/2024.
//

#ifndef BACKGROUNDSCENERYOBJECT_H
#define BACKGROUNDSCENERYOBJECT_H

#include "interfaces/IAnimate.h"
#include "interfaces/IDraw.h"
#include "interfaces/IUpdate.h"

#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Sprite.hpp>

class BackgroundSceneryObject : public IUpdate, public IDraw, public IAnimate {
public:
  sf::Vector2f position;
  float blurRadius = 0.0f;
  BackgroundSceneryObject(TextureLibrary &library, int startIndex, int length, int tick, sf::Vector2f position);
  ~BackgroundSceneryObject() override = default;

  void Update(GameTime gameTime) override;
  void LateUpdate(GameTime gameTime) override;
  void Draw(sf::RenderWindow& window, GameTime gameTime) override;

private:
  sf::Sprite sprite;
  TextureLibrary& library;

  AniSequence sequence;
  sf::Shader blurShader;
};

#endif //BACKGROUNDSCENERYOBJECT_H
