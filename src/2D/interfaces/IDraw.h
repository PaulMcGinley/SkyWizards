//
// Created by Paul McGinley on 30/09/2024.
//

#ifndef IDRAW_H
#define IDRAW_H

#include <SFML/Graphics/RenderWindow.hpp>
#include "models/GameTime.h"
#include "managers/AssetManager.h"

class IDraw {
public:
    virtual ~IDraw() = default;

    virtual void Draw(sf::RenderWindow& window, GameTime gameTime) = 0;

    AssetManager& asset_manager = AssetManager::getInstance();
};

#endif //IDRAW_H
