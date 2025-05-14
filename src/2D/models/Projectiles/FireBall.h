//
// Created by Paul McGinley on 14/05/2025.
//

#ifndef FIREBALL_H
#define FIREBALL_H
#include "models/Projectile.h"


class FireBall : public Projectile {
public:
        FireBall() = default;
        FireBall(sf::Vector2f position, sf::Vector2f velocity, float damage, float speed, float lifetime);
        ~FireBall() override = default;

        void Update(GameTime gameTime) override;
        void Draw(sf::RenderWindow &window, GameTime gameTime) override;
        void LateUpdate(GameTime gameTime) override;
};



#endif //FIREBALL_H
