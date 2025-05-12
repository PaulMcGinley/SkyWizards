//
// Created by Paul McGinley on 12/05/2025.
//

#include "Mob.h"
Mob::Mob(sf::Vector2f position, float viewRange, float moveSpeed, int health)
        : position(position)
        , walkSpeed(moveSpeed)
        , viewRange(viewRange)
        , health(health) {}

int Mob::GetCurrentAnimationFrame() {
        return sequences[currentAnimation].startFrame + currentAnimationFrame + faceDirection;
}