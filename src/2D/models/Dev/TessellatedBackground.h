//
// Created by Paul McGinley on 05/10/2024.
//

#ifndef TESSELLATEDBACKGROUND_H
#define TESSELLATEDBACKGROUND_H

#include <SFML/Graphics/Sprite.hpp>

#include "interfaces/IDraw.h"
#include "interfaces/IUpdate.h"
#include "models/TextureEntry.h"

class TessellatedBackground : public  IDraw, public IUpdate{

public:
    TessellatedBackground();

    TextureEntry mainTex;
    sf::Sprite mainSprite;
    TextureEntry highlightTex;
    sf::Sprite highlightSprite;
    sf::Vector2f position = {0, 0};
    float detailOpacity = 0;

    void Draw(sf::RenderWindow& window, GameTime gameTime) override;
    void Update(GameTime gameTime) override;
    void LateUpdate(GameTime gameTime) override;
    sf::Vector2f clampToPixel(sf::Vector2f position);

};

#endif //TESSELLATEDBACKGROUND_H
