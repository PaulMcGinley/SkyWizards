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
        Projectile(sf::Vector2f position, sf::Vector2f velocity, float damage, float speed, float lifetime, sf::Vector2f collisionPointRight, sf::Vector2f collisionPointLeft);
        ~Projectile() override;

        void Update(GameTime gameTime) override;
        void Draw(sf::RenderWindow &window, GameTime gameTime) override;
        void LateUpdate(GameTime gameTime) override;

        [[nodiscard]] sf::Vector2f GetCollisionPoint() const;

protected:
        sf::Vector2f position;
        sf::Vector2f velocity;
        float damage;
        float speed;
        float lifetime;
        bool facingRight;
        sf::Vector2f collisionPointLeft;
        sf::Vector2f collisionPointRight;
};

#endif //PROJECTILE_H
