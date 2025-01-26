//
// Created by Paul McGinley on 30/09/2024.
//

#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include "enums/FaceDirection.h"
#include "interfaces/IAnimate.h"
#include "interfaces/IDraw.h"
#include "interfaces/IUpdate.h"
#include "models/GameTime.h"
#include "models/Health.h"
#include "models/TextureEntry.h"

class AssetManager;

class Player final : public IAnimate, public IUpdate, public IDraw {
public:
    Player();

    int robeLibrary = 2;
    sf::VertexArray robeQuad{sf::Quads, 4};
    int staffLibrary = 0;
    sf::VertexArray staffQuad{sf::Quads, 4};
    void Update(GameTime gameTime) override;
    void LateUpdate(GameTime gameTime) override;
    void Draw(sf::RenderWindow& window, GameTime gameTime) override;
    FaceDirection faceDirection = FaceDirection::Right;
    sf::Vector2f position = {0, 0};
    int frame() {
        return sequences[currentAni].StartFrame + currentFrame + faceDirection;
    }

        Health health = Health(2.0f, {25, 25});

    void TickAnimation(GameTime gameTime) override;
};



#endif //PLAYER_H
