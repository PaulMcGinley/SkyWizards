//
// Created by Paul McGinley on 14/05/2025.
//

#ifndef PROJECTILE_H
#define PROJECTILE_H
#include "interfaces/IAnimate.h"
#include "interfaces/IDraw.h"
#include "interfaces/IUpdate.h"

class Projectile : public IAnimate, public IDraw, public IUpdate {
public:
        Projectile() = default;
        Projectile(sf::Vector2f position, sf::Vector2f velocity, float damage, float speed, float lifetime, sf::Vector2f collisionPointRight, sf::Vector2f collisionPointLeft, float collisionRadius);
        ~Projectile() override;

        void Update(GameTime gameTime) override;
        void Draw(sf::RenderWindow &window, GameTime gameTime) override;
        void LateUpdate(GameTime gameTime) override;

        virtual int Collide(sf::Vector2f collisionPoint);

        [[nodiscard]] sf::Vector2f GetCollisionCenter() const;
        [[nodiscard]] float GetCollisionRadius() const;
        [[nodiscard]] bool Intersects(const sf::FloatRect& rect) const;
        [[nodiscard]] bool Expired() const;

protected:
        sf::Vector2f position;
        sf::Vector2f velocity;
        float damage;
        float speed;
        float lifetime;
        bool facingRight;
        sf::Vector2f collisionPointLeft;
        sf::Vector2f collisionPointRight;
        float collisionRadius;

};

#endif //PROJECTILE_H
