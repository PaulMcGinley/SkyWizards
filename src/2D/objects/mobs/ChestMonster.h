//
// Created by Paul McGinley on 03/10/2024.
//

#ifndef CHESTMONSTER_H
#define CHESTMONSTER_H
#include <SFML/Graphics/VertexArray.hpp>

#include "enums/FaceDirection.h"
#include "interfaces/IAnimate.h"
#include "interfaces/IDraw.h"
#include "interfaces/IUpdate.h"


class ChestMonster final : public IAnimate, public IUpdate, public IDraw {

public:
    ChestMonster();

    sf::VertexArray texQuad{sf::Quads, 4};
    void Update(GameTime gameTime) override;
    void LateUpdate(GameTime gameTime) override;
    void Draw(sf::RenderWindow& window, GameTime gameTime) override;
    FaceDirection faceDirection = FaceDirection::Left;
    sf::Vector2f position = {0, 0};
    int frame() {
        return sequences[currentAni].StartFrame + currentFrame + faceDirection;
    }

private:
    const float walkSpeed = 100;
    const float runSpeed = 200;
};



#endif //CHESTMONSTER_H
