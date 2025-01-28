//
// Created by Paul McGinley on 27/01/2025.
//

#ifndef LEVEL_H
#define LEVEL_H

#include "interfaces/IDraw.h"
#include "interfaces/IUpdate.h"


class Level : public IUpdate, public IDraw {
public:
        Level() = default;
        ~Level() override = default;

        void update(GameTime gameTime) override;
        void lateUpdate(GameTime gameTime) override;
        void draw(sf::RenderWindow &window, GameTime gameTime) override;

private:
        //std::vector<LevelObject> level_objects; // TODO: Create LevelObject class
        //ILevelObjective level_objective;
        //

};



#endif //LEVEL_H
