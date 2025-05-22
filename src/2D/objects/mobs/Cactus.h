//
// Created by Paul McGinley on 22/05/2025.
//

#ifndef CACTUS_H
#define CACTUS_H
#include "objects/Mob.h"


class Cactus  final : public Mob {
public:
        Cactus(Player* player, sf::Vector2f spawnPosition, float viewRange, float moveSpeed, int health);

        void Update(GameTime gameTime) override;
        void LateUpdate(GameTime gameTime) override;
        void Draw(sf::RenderWindow& window, GameTime gameTime) override;
        void CalculatePhysicsState(std::vector<Boundary> boundaries, GameTime gameTime) override;
        void TickAnimation(GameTime gameTime) override;
        void Damaged(int amount, GameTime gameTime) override;

private:
        void DamagePlayer(int amount) override;

        float nextMoveTime;
        sf::Vector2f nextPosition;
        const float MOVE_LIMIT = 200.f;

        sf::Vector2f GetLeftDropDetectorPosition();
        sf::Vector2f GetRightDropDetectorPosition();
        bool canMoveLeft;
        bool canMoveRight;
        bool onGround;
        const float idleDistance = viewRange;
        const float chaseDistance = viewRange/2;

        void UpdateQuads();
};


#endif //CACTUS_H
