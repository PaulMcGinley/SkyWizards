//
// Created by Paul McGinley on 14/05/2025.
//

#include "Projectile.h"

#include <SFML/Graphics/CircleShape.hpp>

#include "managers/GameManager.h"
Projectile::Projectile(const sf::Vector2f position, const sf::Vector2f velocity, const float damage, const float speed, const float lifetime, sf::Vector2f collisionPointRight, sf::Vector2f collisionPointLeft, float collisionRadius)
        : position(position)
        , velocity(velocity)
        , damage(damage)
        , speed(speed)
        , lifetime(lifetime)
        , facingRight(velocity.x > 0)
        , collisionPointLeft(collisionPointLeft)
        , collisionPointRight(collisionPointRight)
        , collisionRadius(collisionRadius) { /* Nothing in constructor */ }
Projectile::~Projectile() = default;
void Projectile::Update(GameTime gameTime) {
        if (lifetime <= 0) {
                ChangeAnimation(AnimationType::ANIMATION_EXPLOSION, gameTime, true);
                return;
        }
        // Update the position based on the velocity and speed
        position += velocity * speed * gameTime.deltaTime;

        // Decrease the lifetime
        lifetime -= gameTime.deltaTime;
}
void Projectile::Draw(sf::RenderWindow &window, GameTime gameTime) {
        IDraw::Draw(window, "magic", GetTextureDrawIndex(), position);

        // Draw the collision circle
        if(GameManager::GetInstance().ShowCollisions()) {
                sf::CircleShape circle(collisionRadius);
                circle.setFillColor(sf::Color(255, 0, 0, 100));
                circle.setPosition(position - sf::Vector2f(collisionRadius, collisionRadius));
                window.draw(circle);
        }
}
void Projectile::LateUpdate(GameTime gameTime) { TickAnimation(gameTime); }
int Projectile::Collide(sf::Vector2f collisionPoint) {
        velocity = {0, 0};      // Stop the projectile so the explosion animation can play
        lifetime = 1000;        // Set the lifetime to 1 second for the explosion animation (it ends itself before then, this just keep it alive for the explosion)
        position = collisionPoint - sf::Vector2f(400, 350);     // Remove tile size offset

        ChangeAnimation(AnimationType::ANIMATION_EXPLOSION, true);
        assetManager.PlaySoundEffect("Wizard/Magic/collide", 100.f, 3.f);

        return damage;
}
sf::Vector2f Projectile::GetCollisionCenter() const { return velocity.x > 0 ? position + collisionPointRight : position + collisionPointLeft; }
float Projectile::GetCollisionRadius() const { return collisionRadius; }
bool Projectile::Intersects(const sf::FloatRect &rect) const {
        // Get circle center
        sf::Vector2f center = GetCollisionCenter();

        // Find the closest point on rectangle to circle
        float closestX = std::max(rect.left, std::min(center.x, rect.left + rect.width));
        float closestY = std::max(rect.top, std::min(center.y, rect.top + rect.height));

        // Calculate distance between the closest point and circle center
        float distanceX = center.x - closestX;
        float distanceY = center.y - closestY;

        // Compare with circle's radius
        return (distanceX * distanceX + distanceY * distanceY) <= (collisionRadius * collisionRadius);
}
bool Projectile::Expired() const { return lifetime <= 0; }
