//
// Created by Paul McGinley on 12/05/2025.
//

#ifndef MOB_H
#define MOB_H
#include "Enumerators/FaceDirection.h"
#include "interfaces/IAnimate.h"
#include "interfaces/IDraw.h"
#include "interfaces/IUpdate.h"


class Mob : public IAnimate, public IUpdate, public IDraw {
public:
        Mob(sf::Vector2f position, float viewRange, float moveSpeed, int health);

        int GetCurrentAnimationFrame();
        virtual void UpdatePlayerPosition(sf::Vector2f playerPosition) = 0;

protected:
        sf::VertexArray texQuads{sf::Quads, 4};
        FaceDirection faceDirection = FaceDirection::FACE_DIRECTION_LEFT;
        sf::Vector2f position = {0, 0};
        float walkSpeed = 100;
        float runSpeed = 200;
        float viewRange = 0;
        int health = 1;
};



#endif //MOB_H
