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
        ChestMonster(sf::Vector2f position, float viewRange, float moveSpeed, int health);

        void Update(GameTime gameTime) override;
        void LateUpdate(GameTime gameTime) override;
        void Draw(sf::RenderWindow& window, GameTime gameTime)  override;

        void UpdatePlayerPosition(sf::Vector2f playerPosition) override;
};



#endif //CHESTMONSTER_H
