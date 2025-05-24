//
// Created by Paul McGinley on 22/05/2025.
//

#ifndef HAPPYMUSHROOM_H
#define HAPPYMUSHROOM_H

#include "objects/Mob.h"

class HappyMushroom final : public Mob {
public:
        HappyMushroom(Player* player, sf::Vector2f spawnPosition, float viewRange, float moveSpeed, int health);

        void Update(GameTime gameTime) override;
        void LateUpdate(GameTime gameTime) override;
        void Draw(sf::RenderWindow& window, GameTime gameTime) override;
        void CalculatePhysicsState(std::vector<Boundary> boundaries, GameTime gameTime) override;
        void TickAnimation(GameTime gameTime) override;
        void Damaged(int amount, GameTime gameTime) override;

private:
        bool onGround;
        float nextBounceTime;
        static constexpr float BOUNCE_COOLDOWN = 1500; // ms
        const float idleDistance = viewRange;

        void DamagePlayer(int amount) override;
        void UpdateQuads();

        float nextLaughTime = 0; // Time until next laugh sound
};

#endif //HAPPYMUSHROOM_H
