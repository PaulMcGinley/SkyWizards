//
// Created by Paul McGinley on 21/05/2025.
//

#ifndef EYEBALL_H
#define EYEBALL_H

#include "objects/Mob.h"

class EyeBall final : public Mob {
public:
        EyeBall(Player* player, sf::Vector2f spawnPosition, float viewRange, float moveSpeed, int health);

        void Update(GameTime gameTime) override;
        void LateUpdate(GameTime gameTime) override;
        void Draw(sf::RenderWindow& window, GameTime gameTime) override;
        void CalculatePhysicsState(std::vector<Boundary> boundaries, GameTime gameTime) override;
        void DamagePlayer(int amount) override;
        void TickAnimation(GameTime gameTime) override;
        void Damaged(int amount, GameTime gameTime) override;

private:
        float nextAttackTime;
        float nextMoveTime;
        sf::Vector2f spawnPosition;

        float nextGrowlTime = 0; // Time until next growl sound
        float nextTeleportTime = 0;
        void TeleportPlayer();
        bool recentlyTeleported = false;
        void BouncePlayer();
        bool recentlyBounced = false;

        void UpdateQuads();
};

#endif //EYEBALL_H
