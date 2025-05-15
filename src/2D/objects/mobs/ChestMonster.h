//
// Created by Paul McGinley on 03/10/2024.
//

#ifndef CHESTMONSTER_H
#define CHESTMONSTER_H

#include <SFML/Graphics/VertexArray.hpp>
#include "Enumerators/FaceDirection.h"
#include "interfaces/IAnimate.h"
#include "interfaces/IDraw.h"
#include "interfaces/IUpdate.h"
#include "objects/Mob.h"

class ChestMonster final : public Mob {
public:
        ChestMonster(Player* player, sf::Vector2f position, float viewRange, float moveSpeed, int health);

        void Update(GameTime gameTime) override;
        void LateUpdate(GameTime gameTime) override;
        void Draw(sf::RenderWindow& window, GameTime gameTime) override;
        void CalculatePhysicsState(std::vector<Boundary> boundaries, GameTime gametime) override;
        void TickAnimation(GameTime gameTime) override;

private:
        void BitePlayer();
        void SmackPlayer();
        void DamagePlayer(int amount) override;

        float nextSmackTime;
        const float smackCooldown = 5000; // Time between smacks (ms)

        float nextBiteTime;
        const float biteCooldown = 750; // Time between bites (ms)

        sf::Vector2f GetLeftDropDetectorPosition();
        sf::Vector2f GetRightDropDetectorPosition();
        bool canMoveLeft;
        bool canMoveRight;
        bool onGround;
        const float biteDistance = 110.0f;
        const float smackDistance = 200.0f;
        const float idleDistance = viewRange;
        const float chaseDistance = viewRange/2;

        void UpdateQuads();
};



#endif //CHESTMONSTER_H
