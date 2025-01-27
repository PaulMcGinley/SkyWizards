//
// Created by Paul McGinley on 30/09/2024.
//

#ifndef IUPDATE_H
#define IUPDATE_H

#include "models/GameTime.h"

class IUpdate {
public:
    virtual ~IUpdate() = default;

    virtual void update(GameTime gameTime) = 0;
    virtual void lateUpdate(GameTime gameTime) = 0;
};

#endif //IUPDATE_H
