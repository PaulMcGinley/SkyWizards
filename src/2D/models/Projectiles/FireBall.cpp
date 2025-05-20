//
// Created by Paul McGinley on 14/05/2025.
//

#include "FireBall.h"

FireBall::FireBall(const sf::Vector2f position, const sf::Vector2f velocity, const float damage, const float speed, const float lifetime, const float collisionRadius)
        : Projectile(position, velocity, damage, speed, lifetime, {700,450}, {100,450}, collisionRadius) {
        SetAnimationSequences({
                {AnimationType::ANIMATION_PROJECTILE_RIGHT, {0, 40, 15}},
                {AnimationType::ANIMATION_PROJECTILE_LEFT, {40, 40, 15}},
                {AnimationType::ANIMATION_EXPLOSION, {80, 33, 20,nullptr, [this](){this->lifetime = 0;},nullptr}}
        });

        if (velocity.x <= 0) {
                ChangeAnimation(AnimationType::ANIMATION_PROJECTILE_LEFT, true);
        } else {
                ChangeAnimation(AnimationType::ANIMATION_PROJECTILE_RIGHT, true);
        }
}
