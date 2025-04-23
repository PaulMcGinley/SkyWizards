//
// Created by Paul McGinley on 27/01/2025.
//

#ifndef LEVEL_H
#define LEVEL_H

#include "MapObject/WMap.h"
#include "interfaces/IDraw.h"
#include "interfaces/IUpdate.h"


class Level : public IUpdate, public IDraw {
public:
        Level() = default;
        ~Level() override = default;

        void Update(GameTime gameTime) override;
        void LateUpdate(GameTime gameTime) override;
        void Draw(sf::RenderWindow &window, GameTime gameTime) override;

private:
        WMap map;
};



#endif //LEVEL_H
