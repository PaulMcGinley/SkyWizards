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
    ChestMonster();

    sf::VertexArray texture_quads{sf::Quads, 4};

    void update(GameTime gameTime) override;
    void lateUpdate(GameTime gameTime) override;
    void Draw(sf::RenderWindow& window, GameTime gameTime) override;

    FaceDirection face_direction = FaceDirection::FACE_DIRECTION_LEFT;
    sf::Vector2f position = {0, 0};
    int frame() {
        return sequences[current_animation].startFrame + current_animation_frame + face_direction;
    }

private:
    const float WALK_SPEED = 100;
    const float RUN_SPEED = 200;
};



#endif //CHESTMONSTER_H
