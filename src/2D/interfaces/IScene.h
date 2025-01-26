//
// Created by Paul McGinley on 08/10/2024.
//

#pragma once

#ifndef ISCENE_H
#define ISCENE_H

#include "IDraw.h"
#include "IUpdate.h"

class IScene : public IUpdate, public IDraw {
public:
    IScene() = default;
    virtual ~IScene() = default;

        bool Initialized = false;

    void Update(GameTime gameTime) override = 0;
    void LateUpdate(GameTime gameTime) override = 0;
    void Draw(sf::RenderWindow& window, GameTime gameTime) override = 0;

    virtual void Scene_Init() = 0;
    virtual void Scene_Destroy() = 0;
    virtual void OnScene_Active() = 0;
    virtual void OnScene_Deactive() = 0;
};

#endif //ISCENE_H
