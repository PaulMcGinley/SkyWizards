//
// Created by Paul McGinley on 30/09/2024.
//

#ifndef IUPDATE_H
#define IUPDATE_H

#include "managers/InputManager.h"
#include "models/GameTime.h"

class IUpdate {
public:
        virtual ~IUpdate() = default;

        virtual void Update(GameTime gameTime) = 0;
        virtual void LateUpdate(GameTime gameTime) = 0;

        InputManager& inputManager = InputManager::GetInstance();
};

#endif //IUPDATE_H
