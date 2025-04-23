//
// Created by Paul McGinley on 30/09/2024.
//

#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include "Enumerators/FaceDirection.h"
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

        void update(GameTime gameTime) override;
        void lateUpdate(GameTime gameTime) override;
        void Draw(sf::RenderWindow& window, GameTime gameTime) override;
        void TickAnimation(GameTime gameTime) override;

        FaceDirection faceDirection = FaceDirection::FACE_DIRECTION_RIGHT_GENERIC;
        sf::Vector2f position = {0, 0};

        int getCurrentFrame() {
                return sequences[current_animation].startFrame + current_animation_frame + faceDirection;
        }

        Health health = Health(2.0f, {25, 25});

private:
        const int WALKING_SPEED = 128;
        const int RUNNING_SPEED = 400;
};



#endif //PLAYER_H
