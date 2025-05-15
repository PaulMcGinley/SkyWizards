//
// Created by Paul McGinley on 14/05/2025.
//

#include "Projectile.h"
Projectile::Projectile(const sf::Vector2f position, const sf::Vector2f velocity, const float damage, const float speed, const float lifetime, sf::Vector2f collisionPointRight, sf::Vector2f collisionPointLeft)
        : position(position)
        , velocity(velocity)
        , damage(damage)
        , speed(speed)
        , lifetime(lifetime)
        , facingRight(velocity.x > 0)
        , collisionPointLeft(collisionPointLeft)
        , collisionPointRight(collisionPointRight) { /* Nothing in constructor */ }
Projectile::~Projectile() = default;
void Projectile::Update(GameTime gameTime) {
        if (lifetime <= 0) {
                ChangeAnimation(AnimationType::ANIMATION_EXPLOSION, gameTime, true);
                return;
        }
        // Update the position based on the velocity and speed
        position += velocity * speed * gameTime.delta_time;

        // Decrease the lifetime
        lifetime -= gameTime.delta_time;
}
void Projectile::Draw(sf::RenderWindow &window, GameTime gameTime) {
        IDraw::Draw(window, "magic", sequences[currentAnimation].startFrame + currentAnimationFrame, position);
}
void Projectile::LateUpdate(GameTime gameTime) {
        // Update the animation
        TickAnimation(gameTime);
}
int Projectile::Collide() {
        velocity = {0, 0};
        ChangeAnimation(AnimationType::ANIMATION_EXPLOSION, true);
        return damage;
}
sf::Vector2f Projectile::GetCollisionPoint() const {
        return velocity.x > 0 ? collisionPointRight : collisionPointLeft;
}
