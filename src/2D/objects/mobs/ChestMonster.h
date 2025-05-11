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

class ChestMonster final : public IAnimate, public IUpdate, public IDraw {
public:
        ChestMonster(sf::Vector2f position, float viewRange, float moveSpeed, int health);

        sf::VertexArray texture_quads{sf::Quads, 4};

        void UpdateKnowledge(sf::Vector2f playerPosition);
        void Update(GameTime gameTime) override;
        void LateUpdate(GameTime gameTime) override;
        void Draw(sf::RenderWindow& window, GameTime gameTime)  override;

        FaceDirection face_direction = FaceDirection::FACE_DIRECTION_LEFT;
        sf::Vector2f position = {0, 0};
        int frame() {
                return sequences[currentAnimation].startFrame + currentAnimationFrame + face_direction;
        }

private:
        float WALK_SPEED = 100;
        float RUN_SPEED = 200;
        float ViewRange = 0;
        float Health = 1;
};



#endif //CHESTMONSTER_H
