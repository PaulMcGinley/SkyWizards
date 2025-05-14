//
// Created by Paul McGinley on 14/05/2025.
//

#include "FireBall.h"
FireBall::FireBall(const sf::Vector2f position, const sf::Vector2f velocity, const float damage, const float speed, const float lifetime)
        : Projectile(position, velocity, damage, speed, lifetime, {700,450}, {100,450}) {
        sequences = {
                {AnimationType::ANIMATION_PROJECTILE, {0, 40, 60}},
                {AnimationType::ANIMATION_EXPLOSION, {40, 33, 80}}
        };

        ChangeAnimation(AnimationType::ANIMATION_PROJECTILE);
}
void FireBall::Update(GameTime gameTime) {

        Projectile::Update(gameTime);
}
void FireBall::Draw(sf::RenderWindow &window, GameTime gameTime) {

        Projectile::Draw(window, gameTime);
}
void FireBall::LateUpdate(GameTime gameTime) {

        Projectile::LateUpdate(gameTime);
}
