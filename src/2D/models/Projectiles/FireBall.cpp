//
// Created by Paul McGinley on 14/05/2025.
//

#include "FireBall.h"
FireBall::FireBall(const sf::Vector2f position, const sf::Vector2f velocity, const float damage, const float speed, const float lifetime)
        : Projectile(position, velocity, damage, speed, lifetime, {700,450}, {100,450}) {
        sequences = {
                {AnimationType::ANIMATION_PROJECTILE_RIGHT, {0, 40, 20}},
                {AnimationType::ANIMATION_PROJECTILE_LEFT, {40, 40, 20,nullptr, [](){std::cout<<"left";},nullptr}},
                {AnimationType::ANIMATION_EXPLOSION, {80, 33, 80}}
        };

        std::cout << velocity.x << std::endl;
        if (velocity.x <= 0) {
                ChangeAnimation(AnimationType::ANIMATION_PROJECTILE_LEFT, true);
                std::cout << "Left" << std::endl;
        } else {
                ChangeAnimation(AnimationType::ANIMATION_PROJECTILE_RIGHT, true);
        }

}