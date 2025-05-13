//
// Created by Paul McGinley on 12/05/2025.
//

#include "Mob.h"
Mob::Mob(Player *player, sf::Vector2f position, float viewRange, float moveSpeed, int health)
        : position(position)
        , walkSpeed(moveSpeed)
        , viewRange(viewRange)
        , health(health)
        , player(player)
{ /* Nothing in the constructor */}

int Mob::GetCurrentAnimationFrame() {
        return sequences[currentAnimation].startFrame + currentAnimationFrame + faceDirection;
}