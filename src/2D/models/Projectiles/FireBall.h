//
// Created by Paul McGinley on 14/05/2025.
//

#ifndef FIREBALL_H
#define FIREBALL_H
#include "models/Projectile.h"


class FireBall : public Projectile {
public:
        FireBall() = default;
        FireBall(sf::Vector2f position, sf::Vector2f velocity, float damage, float speed, float lifetime, float collisionRadius);
        ~FireBall() override = default;
};



#endif //FIREBALL_H
