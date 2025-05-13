//
// Created by Paul McGinley on 12/05/2025.
//

#ifndef MOB_H
#define MOB_H
#include "Enumerators/FaceDirection.h"
#include "Player.h"
#include "interfaces/IAnimate.h"
#include "interfaces/IDraw.h"
#include "interfaces/IScene.h"
#include "interfaces/IUpdate.h"


class Mob : public IAnimate, public IUpdate, public IDraw {
public:
        Mob(Player* player, sf::Vector2f position, float viewRange, float moveSpeed, int health);

        int GetCurrentAnimationFrame();
        virtual void UpdatePlayerPosition(sf::Vector2f playerPosition, GameTime gameTime) = 0;

protected:
        virtual void DamagePlayer(int amount) = 0;

        sf::VertexArray texQuads{sf::Quads, 4};
        FaceDirection faceDirection = FaceDirection::FACE_DIRECTION_LEFT;
        sf::Vector2f position = {0, 0};
        float walkSpeed = 100;
        float runSpeed = 200;
        float viewRange = 0;
        int health = 1;

        sf::FloatRect collisionBox = {0, 0, 32, 32}; // Collision box for the mob

        IScene* gameScene;
        Player* player;
};



#endif //MOB_H
