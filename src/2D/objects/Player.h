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

        void Update(GameTime gameTime) override;
        void LateUpdate(GameTime gameTime) override;
        void Draw(sf::RenderWindow& window, GameTime gameTime) override;
        void TickAnimation(GameTime gameTime) override;

        FaceDirection faceDirection = FaceDirection::FACE_DIRECTION_RIGHT_GENERIC;
        sf::Vector2f position = {0, 0};

        int getCurrentFrame() {
                return sequences[currentAnimation].startFrame + currentAnimationFrame + faceDirection;
        }

        Health health = Health(2.0f, {25, 25});

        [[nodiscard]] bool IsFalling() const { return isFalling; }
        void SetIsFalling(const bool falling) { isFalling = falling; }

private:
        // Velocity
        sf::Vector2f velocity = {0, 0};
        sf::Vector2f acceleration = {0, 0};
        sf::Vector2f deceleration = {0, 0};
        sf::Vector2f maxVelocity = {400, 800};

        const sf::IntRect collisionBox = {225, 200, 50, 150};
        const sf::Vector2f collisionOffset() { return position + sf::Vector2f(collisionBox.left, collisionBox.top); }
        const float feetPosition() { return position.y + collisionBox.top + collisionBox.height; }

        const int WALKING_SPEED = 128;
        const int RUNNING_SPEED = 400;
        const int FALLING_SPEED = 800;
        const int JUMPING_SPEED = 400;

        bool isFalling = false;
};



#endif //PLAYER_H
